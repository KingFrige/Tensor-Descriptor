## Context

当前 `tensor_descriptor.c` 中的 `microarch_constraints_convert()` 函数强制使用 power-of-2 对齐：

```c
result->desc.unitSkip = (int)next_power_of_2(result->desc.byteNum);
```

这导致量化类型的内存膨胀。本方案通过添加配置选项，允许用户选择是否使用 power-of-2 对齐。

## Goals / Non-Goals

**Goals:**
- 添加 `enablePowerOf2Skip` 配置选项（默认启用，向后兼容）
- 当禁用 power-of-2 时，直接使用 `byteNum` 作为 `unitSkip`
- 重新定义 `hasGap` 含义或保持不变
- 更新所有相关测试用例
- 保持所有现有功能不变

**Non-Goals:**
- 不验证非 power-of-2 skip 的硬件兼容性（由调用方保证）
- 不修改 DMA 驱动或其他硬件相关代码
- 不改变默认行为

## Decisions

### 1. 配置字段设计

```c
// 在 microarch_constraints_t 中添加
typedef struct {
    unsigned int maxByteNum;
    unsigned int maxUnitNum;
    unsigned int maxSliceNum;
    unsigned int maxPlaneNum;
    unsigned int maxCubeNum;
    unsigned int maxTotalBytes;
    unsigned int enableBalance;      // 现有字段
    unsigned int enablePowerOf2Skip; // 新增字段: 0=启用power-of-2(默认), 1=直接使用byteNum
} microarch_constraints_t;
```

### 2. Skip 计算逻辑

```c
// 修改 microarch_constraints_convert() 中的计算
// enablePowerOf2Skip: 0 (default) = power-of-2 mode, 1 = direct mapping mode
if (constraints && constraints->enablePowerOf2Skip == 1) {
    // 直接映射模式
    result->desc.unitSkip = result->desc.byteNum;
    result->hasGap = 0;  // 无间隙
} else {
    // Power-of-2 模式（默认，包括 NULL constraints 或 enablePowerOf2Skip=0）
    result->desc.unitSkip = (int)next_power_of_2(result->desc.byteNum);
    unsigned int actualStride = next_power_of_2(dim[0]);
    result->hasGap = (actualStride > dim[0]) ? 1 : 0;
}

// 上层 skip 计算保持不变
result->desc.sliceSkip = result->desc.unitNum * result->desc.unitSkip;
result->desc.planeSkip = result->desc.sliceNum * result->desc.sliceSkip;
result->desc.cubeSkip = result->desc.planeNum * result->desc.planeSkip;
```

### 3. hasGap 语义

- **Power-of-2 模式**: hasGap=1 表示存在对齐间隙
- **直接映射模式**: hasGap=0 表示无间隙

### 4. 文件修改清单

| 文件 | 修改内容 |
|------|----------|
| `src/tensor_descriptor.h` | 添加 `enablePowerOf2Skip` 字段到结构体 |
| `src/tensor_descriptor.c` | 修改 skip 计算逻辑，添加条件判断 |
| `tests/constraint_test.c` | 更新测试用例，测试两种模式 |
| `tests/batch_constraint_test.c` | 更新测试用例 |
| `tests/utils.c` | 更新 `microarch_tensor_create_random()` |

### 5. 测试策略

1. **默认行为测试**: 不设置新字段，验证行为不变
2. **Power-of-2 模式测试**: `enablePowerOf2Skip=1`，验证现有行为
3. **直接映射模式测试**: `enablePowerOf2Skip=0`，验证新行为
4. **内存占用验证**: 对比两种模式的内存占用

## Migration Plan

1. **阶段 1**: 添加配置字段到头文件
2. **阶段 2**: 修改转换函数逻辑
3. **阶段 3**: 更新测试用例
4. **阶段 4**: 运行测试验证
5. **阶段 5**: 文档更新

## Risks / Trade-offs

| 风险 | 缓解措施 |
|------|----------|
| 向后兼容性破坏 | 默认启用 power-of-2，行为不变 |
| 非 power-of-2 导致 DMA 错误 | 由调用方验证硬件兼容性 |
| 测试覆盖不足 | 为两种模式分别创建测试用例 |

## Implementation Checklist

- [ ] 修改 `src/tensor_descriptor.h`: 添加 `enablePowerOf2Skip` 字段
- [ ] 修改 `src/tensor_descriptor.c`: 实现条件 skip 计算
- [ ] 更新 `tests/constraint_test.c`: 添加新测试用例
- [ ] 更新 `tests/batch_constraint_test.c`: 添加新测试用例
- [ ] 更新 `tests/utils.c`: 初始化新字段
- [ ] 运行所有测试验证
