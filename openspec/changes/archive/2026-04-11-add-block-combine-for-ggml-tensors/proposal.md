# Proposal: Add Block Combine for GGML Tensors

## Why

当前 `ggml_to_microarch_direct_map` 将每个 GGML block 映射为一个 microarch unit，导致：

- **访问次数过多**：Q4_0 tensor (4096×151936) 产生 1940 万次 unit 访问
- **效率低下**：小 byteNum (18) 导致频繁的 DMA 传输启动
- **硬件利用率不足**：无法充分利用 DMA 的批量传输能力

通过将多个 blocks 组合成更大的 unit，可以：
- 减少 50-67% 的访问次数
- 提高 DMA 传输效率
- 保持数据连续性（无需复制）

## What Changes

- **ADD**: `ggml_block_combine_config_t` 配置结构体
- **MODIFY**: `ggml_to_microarch_direct_map()` 添加 block 组合功能
- **ADD**: 自动寻找最优 `target_byteNum` 逻辑
- **ADD**: 使用 `tests/ggml_tensor_test_data.inc` 全量测试
- **ADD**: 覆盖 `ggml_tensor.h` 中所有类型组合的测试

## Implementation Strategy

### 核心算法
```
target_byteNum = floor(max_byteNum / type_size) * type_size
blocks_per_unit = target_byteNum / type_size
unitNum = ceil(total_blocks / blocks_per_unit)  // 向上取整
```

### 设计决策
1. **向上取整**：允许最后一个 unit 超界访问，简化逻辑
2. **自动优化**：从 64 向下寻找最大可整除值
3. **零拷贝**：依赖连续内存布局（已验证）

## Capabilities

- `ggml-block-combine`: 将多个 GGML blocks 组合为更大的 microarch unit

## Impact

- **API 变更**: `ggml_to_microarch_direct_map()` 添加可选参数
- **性能提升**: Q4_0 减少 66% 访问次数，Q8_0 减少 50%
- **测试覆盖**: 102 个全量 tensor 测试
- **向后兼容**: config=NULL 时保持原有行为
