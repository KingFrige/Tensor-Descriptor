# TODO

1. 深入理解 llama.cpp 中ggml_tensor
  - 文档： docs/ggml_tensor.md
  - tmp/llama.cpp/ggml/src/ggml-common.h
  - tmp/llama.cpp/ggml/include/ggml.h

1. 深入理解 microarch_tensor: docs/microarch_tensor.md

1. 深入理解 ggml_tensor 相关例子:
  - assets/qwen_tensor.txt
  - assets/ggml_tensors_qwen3_models.json
1. 深入理解 src下的代码

## 需求

1. ~~整理 ggml_tensor 逻辑，提取 tmp/llama.cpp/ggml/src/ggml-common.h / tmp/llama.cpp/ggml/include/ggml.h~~ ✅
1. ~~避免与 tmp/llama.cpp 依赖~~ ✅
1. ~~参考 tests/utils.c: int* microarch_tensor_traversal, 实现 ggml_tensor_traversal~~ ✅
  - ~~microarch_tensor_traversal 底层遍历 unit~~ ✅
  - ~~ggml_tensor_traversal 底层遍历 block~~ ✅
  - ~~测试数据来源 docs/qwen_tensor.json~~ ✅

1. ~~注意 type_size / block_size 等对 dimensions / strides (ne/nb) 的影响~~ ✅


## 测试

1. ggml_tensor 例子：
  - docs/qwen_tensor.json
