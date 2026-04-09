# Tensor-Descriptor 项目指南

## 项目概述

Tensor-Descriptor 是一个用于处理张量描述符和地址生成的 C++ 项目。该项目支持在张量数据的不同抽象级别（架构层和微架构层）上生成和遍历地址，适用于 AI 加速器或 SIMD 架构的内存访问模式分析和测试。

### 核心功能

- **架构层张量描述符（archTensor）**: 5维张量抽象，支持父张量和子张量的定义与转换
- **微架构层张量描述符（microarchTensor）**: 底层内存布局描述，支持地址遍历和生成
- **JSON 配置支持**: 通过配置文件注入测试参数
- **地址遍历**: 支持多级（cube/plane/slice/unit）地址点遍历

## 项目结构

```
/home/korben/workspace/project/R100/Tensor-Descriptor/
├── src/                    # 头文件目录
│   ├── arch_tensor.hh     # 架构层张量描述符类定义
│   └── microarch_tensor.hh # 微架构层张量描述符类定义
├── tests/                  # 测试和主程序入口（已从 src/ 迁移）
│   ├── arch_main.cc       # 架构层测试程序
│   ├── microarch_main.cc  # 微架构层测试程序
│   └── microarch_tool.cc  # JSON配置测试工具
├── configs/               # 配置文件目录
│   └── demo.json          # 示例张量描述符配置
├── docs/                  # 文档目录
│   └── cube.svg           # 张量结构示意图
├── ext/                   # 外部依赖
│   └── nlohmann_json/     # JSON 处理库
├── build/                 # 构建输出目录（CMake 生成）
├── CMakeLists.txt         # CMake 构建配置
├── README.md              # 项目说明文档
├── TODO.md                # 待办事项
└── AGENTS.md              # 本文件

```

## 架构说明

### 1. 微架构层（microarchTensor）

微架构层描述张量在硬件层面的内存布局：

**维度定义**（从细到粗）：
- `byteNum`: 每个单元的字节数（0 < byteNum < 64）
- `unitNum`: 单元数量
- `sliceNum`: 切片数量
- `planeNum`: 平面数量
- `cubeNum`: 立方体数量

**步长计算**（Skip/Stride）：
```cpp
unitSkip  = 1 << ceil(log2(byteNum))  // 对齐到2的幂
sliceSkip = unitNum  * unitSkip
planeSkip = sliceNum * sliceSkip
cubeSkip  = planeNum * planeSkip
```

**SRAM 地址格式**：
```
[5:0]   - byteIdx  (字节索引)
[19:6]  - entryIdx (条目索引)
[22:20] - bankIdx  (存储体索引)
```

### 2. 架构层（archTensor）

架构层提供更高层次的5维张量抽象：

**张量描述符**（archTensorDescriptor）：
- `baseAddr`: 基地址
- `dimension[5]`: 5维尺寸（对应 byte/unit/slice/plane/cube）
- `stride[5]`: 各维度的步长

**子张量描述符**（subArchTensorDescriptor）：
- `coords[5]`: 子张量起始坐标
- `range[5]`: 子张量在各维度的范围
- `traversalStride[5]`: 遍历步长

**转换机制**:
- `convertTesor2MicroarchTensorDescriptor()`: 将完整张量转换为微架构描述符
- `convertSubTesor2MicroarchTensorDescriptor()`: 将子张量转换为微架构描述符

## 构建与运行

### 前置要求

- CMake 3.0.0 或更高版本
- C++17 兼容的编译器
- nlohmann_json 库（已作为子模块包含）

### 构建步骤

```bash
cd build
cmake ..
make
```

### 生成的可执行文件

构建完成后，在 `build/` 目录下生成：

| 可执行文件 | 源文件 | 功能描述 |
|-----------|--------|----------|
| `archTensorDescriptor` | `tests/arch_main.cc` | 测试架构层张量描述符，生成随机张量并转换为微架构层进行遍历 |
| `microarchTensorDescriptor` | `tests/microarch_main.cc` | 测试微架构层张量描述符，生成随机张量并遍历地址 |
| `microarchTool` | `tests/microarch_tool.cc` | 从 JSON 配置文件读取张量参数，进行地址遍历测试 |

### 运行测试

```bash
# 测试架构层（随机生成张量）
./archTensorDescriptor

# 测试微架构层（随机生成张量）
./microarchTensorDescriptor

# 使用 JSON 配置测试
./microarchTool
```

### JSON 配置格式

`configs/demo.json` 示例：

```json
[
  {
    "baseAddr": 0,
    "byteNum": 64,
    "unitNum": 28,
    "sliceNum": 4,
    "planeNum": 28,
    "cubeNum": 1,
    "unitSkip": 64,
    "sliceSkip": 1792,
    "planeSkip": 7168,
    "cubeSkip": 0
  }
]
```

## 开发约定

### 代码风格

- **命名规范**: 
  - 类名使用驼峰命名法（如 `microarchTensor`, `archTensor`）
  - 成员变量使用小驼峰（如 `tensorDesc`, `baseAddr`）
  - 结构体成员使用小驼峰（如 `byteNum`, `unitSkip`）

- **头文件组织**: 所有实现位于 `.hh` 头文件中，测试程序使用 `.cc` 源文件

- **随机数据生成**: 使用 `genRandomData()` 方法生成测试数据，基于当前时间作为随机种子

### 编译选项

所有目标使用以下编译选项：
```cmake
-Wall -Wextra -g -O0 -ggdb
```

- `-Wall -Wextra`: 启用所有警告
- `-g -O0 -ggdb`: 调试信息，无优化

## 已知问题和待办事项

### 当前状态

- 源文件已从 `src/` 迁移至 `tests/`，原 `src/arch_main.cc` 等文件被删除
- 当前依赖 `nlohmann/json.hpp` 进行 JSON 解析

### 计划改进

1. **C 语言重写**: 将 C++ 代码重写为 C 代码，去除 C++ 特性依赖
2. **移除 JSON 库依赖**: 实现自定义 JSON 解析器，直接读取配置文件
3. **测试覆盖**: 完善测试用例，验证地址计算的正确性

## 关键类和方法

### microarchTensor

| 方法 | 功能 |
|------|------|
| `microarchTensor()` | 默认构造函数，生成随机测试数据 |
| `microarchTensor(...)` | 参数化构造函数，用于从 JSON 配置创建 |
| `traversalTensorAddrPoint()` | 遍历张量所有地址点，返回地址数组 |
| `getTraversalAddrPointNum()` | 计算需要遍历的地址点总数 |
| `genSubTensor()` | 生成子张量描述符 |
| `getSkipAddr()` | 计算跳转地址，处理地址回绕 |

### archTensor

| 方法 | 功能 |
|------|------|
| `archTensor()` | 构造函数，随机初始化 5D 张量描述符 |
| `genSubTensor()` | 生成随机子张量配置 |
| `convertTesor2MicroarchTensorDescriptor()` | 将架构张量转换为微架构描述符 |
| `convertSubTesor2MicroarchTensorDescriptor()` | 将子张量转换为微架构描述符 |

## 调试和日志

项目使用标准输出（`std::cout`）进行调试信息输出，包括：

- 张量描述符的完整信息
- 遍历过程中的层级计数（cubeCnt, planeCnt, sliceCnt, unitCnt）
- 地址索引和十六进制地址值
- 层级完成标记（`***** slice done ******`, `----- plane done ------`, `===== cube done ======`）

## 依赖说明

- **nlohmann_json**: MIT 许可证，用于 JSON 文件解析（计划未来移除）
- **标准库**: `<iostream>`, `<cmath>`, `<cstdlib>`, `<ctime>`, `<iomanip>`, `<sstream>`, `<fstream>`, `<memory>`

---

*最后更新: 2026-04-09*
