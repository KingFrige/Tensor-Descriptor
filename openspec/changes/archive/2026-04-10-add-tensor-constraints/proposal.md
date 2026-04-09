## Why

当前 arch_tensor 转换为 microarch_tensor 时没有硬件约束检查。真实硬件场景中 byteNum、unitNum、sliceNum、planeNum、cubeNum 存在物理限制，需要在转换时验证约束并处理冲突，否则会产生无效的张量描述符。

## What Changes

- 新增 `microarch_constraints_t` 结构体，支持用户传入各维度约束（0=无约束）
- byteNum 硬编码最大值为 64（硬件限制）
- 转换函数增加约束检查和自动调整逻辑
- 新增间隙检测标志 `hasGap`：无间隙时 arch_total == microarch_total，有间隙时 arch_total < microarch_total
- 约束冲突时返回错误码，不做自动调整
- 子张量转换复用相同的约束系统

## Capabilities

### New Capabilities
- `tensor-constraints`: 张量约束系统，支持约束定义、验证、冲突检测

## Impact

- 修改 `src/arch_tensor.h` 和 `src/arch_tensor.c`：新增约束结构和转换函数
- 修改 `src/microarch_tensor.h` 和 `src/microarch_tensor.c`：新增间隙检测逻辑
- 新增 `tests/constraint_test.c`：充分测试约束场景