# TODO

- [x] 遍历 tests/ggml_tensor_test_data.inc:ggml_test_tensor_info_t ggml_test_tensors[] 测试 ggml_to_microarch_direct_map
  - 已实现：在 tests/ggml_tensor_traversal_test.c 中添加 test_ggml_to_microarch_map() 函数
  - 打印 GGML tensor 信息（ne, nb, type, type_size, blck_size）
  - 打印映射后的 microarch_tensor 信息（byteNum, unitNum, skip 等）
  - 验证映射正确性（20/20 测试通过）

## Test Command

```bash
./build.sh -a
# or
cd build && cmake .. && make ggml_tensor_traversal_test && ./ggml_tensor_traversal_test
```
