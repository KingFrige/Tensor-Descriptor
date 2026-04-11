## Why

当前 `tensor_descriptor` 强制使用 power-of-2 对齐的 unitSkip，导致量化类型（如 Q4_0）内存占用膨胀严重。例如：

- Q4_0: byteNum=18 → unitSkip=32，膨胀率 1.78x
- Q8_0: byteNum=34 → unitSkip=64，膨胀率 1.88x

为了优化内存使用，需要支持非 power-of-2 的 unitSkip，同时保持向后兼容。

## What Changes

- **ADD**: 在 `microarch_constraints_t` 中添加 `enablePowerOf2Skip` 配置选项
- **MODIFY**: `microarch_constraints_convert()` 函数根据配置选择 skip 计算方式
- **MODIFY**: `hasGap` 计算逻辑，适应新的 skip 模式
- **UPDATE**: 相关测试用例支持新的配置选项
- **COMPATIBILITY**: 默认启用 power-of-2（向后兼容）

## Capabilities

### New Capabilities
- `non-power-of-2-skip`: 支持直接使用 byteNum 作为 unitSkip，无需对齐到 power-of-2

### Modified Capabilities
- `tensor-constraints`: 新增配置字段 `enablePowerOf2Skip`
- `tensor-conversion`: 转换函数支持两种 skip 计算模式

## Impact

- **API 变更**: `microarch_constraints_t` 结构体新增字段
- **功能影响**: 无破坏性变更，默认行为保持不变
- **测试更新**: 需要更新测试用例验证新配置
- **内存优化**: 启用新选项后，量化类型内存占用降低 ~44%
