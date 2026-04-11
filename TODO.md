# TODO

1. 深入理解 llama.cpp 中ggml_tensor
  - 文档： docs/ggml_tensor.md
  - tmp/llama.cpp/ggml/src/ggml-common.h
  - tmp/llama.cpp/ggml/include/ggml.h

1. 深入理解 ggml_tensor tese: tests/ggml_tensor_traversal_test.c
1. 深入理解 microarch_tensor: docs/microarch_tensor.md

1. 深入理解 ggml_tensor 相关例子:
  - assets/qwen_tensor.txt
  - assets/ggml_tensors_qwen3_models.json
1. 深入理解 src下的代码
1. 深入理解 docs/mapping.md, 理解映射关系
   - n 个 block 对应 一个unit
   - 根据block_size / type_size 动态调整 底层 byteNum 大小

## 需求

1. ~~探索清楚~~ ✅
   - ~~ne[0] -> block~~ ✅
   - ~~block -> unit~~ ✅
   - ~~ne[1] -> slice~~ ✅
   - ~~ne[2] -> plane~~ ✅
   - ~~ne[3] -> cube~~ ✅
   - ~~nb -> skip 关系: 连续模式 / 非连续模式~~ ✅

1. ~~更新src下的映射，实现ggml_tensor 到 microarch_tensor 的 的正确映射~~ ✅
   - ~~添加 `enablePowerOf2Skip` 配置选项~~ ✅
   - ~~实现非 power-of-2 的 unitSkip 直接映射~~ ✅
   - ~~向后兼容：默认使用 power-of-2 模式~~ ✅
1. ~~使用 tests/ggml_tensor_test_data.inc 中的case注入到新的映射中，测试实现~~ ✅
   - ~~约束测试通过 (24/24)~~ ✅


