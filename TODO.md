# TODO

## 多个block组合成unit - 已完成 ✅

### 实现概述
- 添加了 `ggml_block_combine_config_t` 配置结构体
- 扩展了 `ggml_to_microarch_direct_map()` 函数，支持 block 组合
- 自动选择最优 `target_byteNum` (floor(64/type_size)*type_size)
- 支持用户指定目标组合比例
- 保持向后兼容 (NULL config = 自动选择)

### 性能提升
- **Q4_0**: 66.4% 访问减少 (1940万 → 637万次)
- **Q4_0 (强制 36)**: 50% 访问减少 (1940万 → 972万次)
- **F32**: 256 units (每 unit 16个元素)

### 测试结果
- constraint_test: 24/24 passed ✅
- batch_constraint_test: 128/128 passed ✅
- ggml_tensor_traversal_test: All tests passed ✅
- Block combine config tests: 5/5 passed ✅

## Test Command

```bash
./build.sh -a
# or
cd build && cmake .. && make ggml_tensor_traversal_test && ./ggml_tensor_traversal_test
```
