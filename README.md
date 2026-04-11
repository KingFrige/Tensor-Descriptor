# Tensor Descriptor

C11 张量描述符和地址生成库。

## 快速开始

```bash
cd build
cmake ..
make
```

### 运行测试

```bash
./build/constraint_test    # 单项测试
./build/batch_constraint_test  # 批量测试
```

## 架构

```
┌─────────────────────────────────────────────────────────────┐
│                    公开 API                                 │
│              tensor_descriptor_convert()                    │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                   核心库 (src/)                          │
│    tensor_descriptor.c / tensor_descriptor.h              │
│    - tensor_descriptor_convert()                         │
│    - tensor_descriptor_convert_sub()                     │
│    - microarch_constraints_convert()                  │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                  测试工具 (tests/)                        │
│              utils.c / utils.h                           │
│    - ggml_tensor_create_random()                        │
│    - microarch_tensor_create_random()                  │
│    - microarch_tensor_traversal()                      │
└─────────────────────────────────────────────────────────────┘
```

## 约束系统

### 1. 需求侧约束 (User Constraints)

用户可通过 `tensor_constraints_t` 结构体传入约束条件：

| 字段 | 说明 |
|------|------|
| `maxByteNum` | 最大字节数 |
| `maxUnitNum` | 最大单元数 |
| `maxSliceNum` | 最大切片数 |
| `maxPlaneNum` | 最大平面数 |
| `maxCubeNum` | 最大立方体数 |
| `maxTotalBytes` | 最大总字节数 |
| `enableBalance` | 启用均衡模式，使 micro/arch 维度乘积比接近 1 |

### 2. 物理限制 (Physical Limits)

物理限制通过 `tensor_physical_limits_t` 结构体定义：

| 字段 | 默认最大值 | 说明 |
|------|-----------|------|
| `maxPhysicalByteNum` | 64 | 单个元素最大字节数 |
| `maxPhysicalUnitNum` | 1024 | 最大单元数 |
| `maxPhysicalSliceNum` | 1024 | 最大切片数 |
| `maxPhysicalPlaneNum` | 1024 | 最大平面数 |
| `maxPhysicalCubeNum` | 1024 | 最大立方体数 |

### 3. 维度自动扩展

当维度超出物理限制时，自动扩展到下一个维度：

```
溢出映射规则:
1. 总量约束: arch维度乘积 ≤ micro维度乘积
2. 约束计算 (取较小值):
   - byteNum  = min(需求约束, 物理限制)
   - unitNum  = min(需求约束, 物理限制)
   - sliceNum = min(需求约束, 物理限制)
   - planeNum = min(需求约束, 物理限制)
   - cubeNum  = min(需求约束, 物理限制)
3. 溢出从低维度开始
4. 均衡: micro维度乘积 / arch维度乘积 接近 1
5. 超约束时报错
```

## API 接口

### 核心类型

```c
// 描述符
typedef struct {
    unsigned int baseAddr;
    unsigned int dimension[5];  // [byte, unit, slice, plane, cube]
    unsigned int stride[5];
} tensor_descriptor_t;

// 转换结果
typedef struct {
    int baseAddr;
    int byteNum;
    int unitNum;
    int sliceNum;
    int planeNum;
    int cubeNum;
    int unitSkip;
    int sliceSkip;
    int planeSkip;
    int cubeSkip;
    int errorCode;     // 错误码
    unsigned int effectiveMaxByte;
    unsigned int effectiveMaxUnit;
    unsigned int effectiveMaxSlice;
    unsigned int effectiveMaxPlane;
    unsigned int effectiveMaxCube;
} tensor_conversion_result_t;

// 约束条件
typedef struct {
    unsigned int maxByteNum;
    unsigned int maxUnitNum;
    unsigned int maxSliceNum;
    unsigned int maxPlaneNum;
    unsigned int maxCubeNum;
    unsigned int maxTotalBytes;
    unsigned int enableBalance;
} tensor_constraints_t;

// 物理限制
typedef struct {
    unsigned int maxPhysicalByteNum;
    unsigned int maxPhysicalUnitNum;
    unsigned int maxPhysicalSliceNum;
    unsigned int maxPhysicalPlaneNum;
    unsigned int maxPhysicalCubeNum;
} tensor_physical_limits_t;
```

### 错误码

| 错误码 | 说明 |
|--------|------|
| `E_SUCCESS` (0) | 成功 |
| `E_BYTE_NUM_EXCEEDED` (-1) | 字节数超出 |
| `E_OVER_CONSTRAINED` (-2) | 约束过强 |
| `E_INVALID_DIMENSION` (-3) | 无效维度 |
| `E_PHYSICAL_CONSTRAINT` (-4) | 物理限制冲突 |

### 公开函数

```c
// 主转换函数
int tensor_descriptor_convert(const tensor_descriptor_t* desc,
                               const tensor_constraints_t* constraints,
                               const tensor_physical_limits_t* limits,
                               tensor_conversion_result_t* result);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `desc` | 输入的架构层张量描述符 |
| `constraints` | 需求侧约束条件，可为 NULL |
| `limits` | 物理限制，可为 NULL |
| `result` | 转换结果 |

**返回值：**

- 成功返回 `E_SUCCESS`
- 失败返回错误码

**使用示例：**

```c
#include "tensor_descriptor.h"

// 1. 创建描述符
tensor_descriptor_t desc = {
    .baseAddr = 5120,
    .dimension = {64, 100, 1, 1, 1},
    .stride = {64, 6400, 6400, 6400, 6400}
};

// 2. 设置约束 (可选)
tensor_constraints_t constraints = {
    .maxUnitNum = 50,
    .maxTotalBytes = 0,
    .enableBalance = 1  // 启用均衡模式
};

// 3. 设置物理限制 (可选)
tensor_physical_limits_t limits = {
    .maxPhysicalByteNum = 64,
    .maxPhysicalUnitNum = 1024
};

// 4. 执行转换
tensor_conversion_result_t result;
int ret = tensor_descriptor_convert(&desc, &constraints, &limits, &result);

// 5. 检查结果
if (ret == E_SUCCESS) {
    printf("byteNum=%d, unitNum=%d, sliceNum=%d\n",
           result.desc.byteNum, result.desc.unitNum, result.desc.sliceNum);
}
```

## Microarch Tensor 结构

### SRAM 地址编码

```
[5:0]   - byteIdx
[19:6]  - entryIdx
[22:20] - bankIdx
```

### 维度说明

- **byteNum**: 单元素字节数 (0 < byteNum <= 64)
- **unitNum**: 单元数
- **sliceNum**: 切片数
- **planeNum**: 平面数
- **cubeNum**: 立方体数

### Skip 计算

```c
// 整型实现 (无需 math.h)
static unsigned int next_power_of_2(unsigned int x) {
    if (x == 0) return 1;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

unitSkip  = next_power_of_2(byteNum);
sliceSkip = unitNum * unitSkip;
planeSkip = sliceNum * sliceSkip;
cubeSkip  = planeNum * planeSkip;
```
