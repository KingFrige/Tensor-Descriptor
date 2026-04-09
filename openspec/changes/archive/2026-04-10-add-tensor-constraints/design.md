## Context

当前项目实现了 arch_tensor（高层5维张量）和 microarch_tensor（底层硬件描述）两种表示，以及它们之间的转换函数 `arch_tensor_convert_to_microarch()`。

但转换函数没有考虑硬件约束：
- byteNum 可能超过硬件限制（最大64）
- 各维度（unitNum/sliceNum/planeNum/cubeNum）可能超过硬件能力
- 没有间隙检测（内存对齐导致的浪费）
- 没有约束冲突检测

## Goals / Non-Goals

**Goals:**
- 实现 `microarch_constraints_t` 结构体，支持用户传入约束（0=无约束）
- 实现约束检查和自动调整逻辑
- 实现间隙检测（hasGap 标志）
- 实现过约束检测并返回错误

**Non-Goals:**
- 不实现约束的持久化存储（仅内存）
- 不实现约束的自动求解器（仅简单调整）
- 不修改现有的随机数据生成逻辑

## Decisions

### 1. 约束结构设计
```c
typedef struct {
    unsigned int maxUnitNum;    // 0 = 无约束
    unsigned int maxSliceNum;    // 0 = 无约束
    unsigned int maxPlaneNum;    // 0 = 无约束
    unsigned int maxCubeNum;     // 0 = 无约束
    unsigned int maxTotalBytes;  // 0 = 无约束
} microarch_constraints_t;
```
**决策**: 约束通过结构体传入，0表示无约束

### 2. 转换结果结构
```c
typedef struct {
    microarch_tensor_descriptor_t desc;
    int hasGap;       // 0=无间隙, 1=有间隙
    int errorCode;    // 0=成功, 负数=错误
} microarch_conversion_result_t;
```
**决策**: 结果包含 descriptor、间隙标志、错误码

### 3. 调整优先级
**决策**: 调整优先级从低到高为 unitNum → sliceNum → planeNum → cubeNum
**理由**: 高优先级维度更难调整（如 cube 通常代表物理结构）

### 4. 错误码定义
```c
#define E_SUCCESS           0
#define E_BYTE_NUM_EXCEEDED  -1   // byteNum > 64
#define E_OVER_CONSTRAINED  -2   // 约束无法同时满足
#define E_INVALID_DIMENSION -3   // 维度为0
```
**决策**: 使用负数错误码，0表示成功

## Risks / Trade-offs

| 风险 | 影响 | 缓解 |
|------|------|------|
| 调整算法可能无法找到有效解 | 中 | 返回 E_OVER_CONSTRAINED 错误 |
| 间隙计算涉及浮点数 | 低 | 使用整数位移运算代替 log2 |
| 子张量转换复用逻辑复杂性 | 中 | 抽取公共辅助函数 |

## Migration Plan

1. 在 `arch_tensor.h` 中添加新结构体和函数声明
2. 在 `arch_tensor.c` 中实现约束转换逻辑
3. 添加 `tests/constraint_test.c` 测试各种约束场景
4. 验证所有现有测试仍然通过

## Open Questions

1. 约束默认值：是否需要提供一个默认约束结构？
2. 错误消息：是否需要详细的错误描述字符串？
3. 性能：约束检查的开销是否可接受？