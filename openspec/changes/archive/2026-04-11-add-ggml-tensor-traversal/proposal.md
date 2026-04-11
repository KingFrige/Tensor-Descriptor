## Why

当前项目已实现了 microarch_tensor 的遍历功能（`microarch_tensor_traversal`），用于在微架构层遍历 tensor 内存地址。为了支持 GGML（Georgi Gerganov Machine Learning）张量格式的遍历，需要实现类似的遍历功能。

GGML 张量格式广泛应用于 llama.cpp 等推理框架，支持多种量化类型（Q4_0, Q8_0 等）。与 microarch_tensor 不同，GGML 张量：
- 使用 4D 维度（ne[0-3]）而非 5D
- 使用 stride-based 寻址（nb[0-3]）而非 skip-based
- 支持量化类型，需要按 block 而非 element 遍历
- 有 type_size 和 blck_size 概念

## What Changes

- **新增**: `src/ggml_tensor.h` - GGML 张量核心数据类型定义（独立文件，无 llama.cpp 依赖）
- **新增**: `src/ggml_tensor.c` - GGML 张量遍历实现
- **新增**: `tests/ggml_tensor_traversal_test.c` - 独立测试程序（硬编码测试数据）
- **新增**: `scripts/extract_qwen_tensor.py` - 从 docs/qwen_tensor.json 提取测试数据的 Python 脚本
- **更新**: `CMakeLists.txt` - 添加新的测试目标

## Capabilities

### New Capabilities
- `ggml-tensor-traversal`: GGML 张量块级遍历，返回 element 和 block 信息
- `ggml-type-support`: 支持 F32, F16, Q4_0, Q8_0 等类型的 type_size/blck_size 查询

### Modified Capabilities
- （无现有规范需要修改，这是独立新增功能）

## Impact

- **新增文件**: 
  - `src/ggml_tensor.h` - 约 100 行（数据类型定义）
  - `src/ggml_tensor.c` - 约 200 行（遍历实现）
  - `tests/ggml_tensor_traversal_test.c` - 约 150 行（测试程序）
  - `scripts/extract_qwen_tensor.py` - 约 50 行（数据提取脚本）
- **构建系统**: CMakeLists.txt 添加新的测试目标
- **依赖**: 无外部依赖，完全独立于 llama.cpp
- **API**: 新增遍历 API，不影响现有 microarch_tensor API
