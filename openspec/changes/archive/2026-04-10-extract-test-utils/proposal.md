## Why

测试代码（tests/*.c）中需要使用 `arch_tensor_create_random`、`microarch_tensor_create_random` 等函数创建测试用的张量。目前这些函数定义在 `src/tensor_descriptor.c` 中，属于核心库代码。

将测试工具函数提取到独立的 `tests/utils.c/h` 模块，可以让测试代码更清晰，也便于后续添加更多测试工具函数。

## What Changes

- 新增 `tests/utils.h`：声明测试工具函数
- 新增 `tests/utils.c`：实现测试工具函数
- 将以下函数从 `src/tensor_descriptor.c` 移动到 `tests/utils.c`：
  - `arch_tensor_create_random()`
  - `microarch_tensor_create_random()`
  - `microarch_tensor_create_from_desc()`
  - `arch_tensor_gen_sub_tensor()`
  - `microarch_tensor_gen_sub_tensor()`
  - `microarch_tensor_print()`
  - 相关的静态 helper 函数（`gen_random_data`, `max_val`, `compute_arch_strides`, `get_skip_addr`）
- 更新 `src/tensor_descriptor.c`：移除上述函数
- 更新 `CMakeLists.txt`：添加 tests/utils.c 到构建

## Capabilities

### Test Utilities
- `tests/utils.h/c`：独立的测试工具模块，提供随机张量生成、子张量生成、打印等功能

## Impact

- 新增 `tests/utils.h` 和 `tests/utils.c`
- 修改 `src/tensor_descriptor.c`：移除测试相关函数
- 修改 `CMakeLists.txt`：构建配置更新