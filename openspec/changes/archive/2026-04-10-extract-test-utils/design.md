# Design: 提取测试工具函数

## 目标

将测试相关的辅助函数从核心库代码 (`src/tensor_descriptor.c`) 提取到独立的测试工具模块 (`tests/utils.c/h`)。

## 问题分析

当前 `src/tensor_descriptor.c` 包含两类函数：

1. **核心库函数**：张量转换、约束处理（给库用户使用）
2. **测试工具函数**：仅供测试使用，不应出现在核心库中

测试工具函数列表 (在 `tensor_descriptor.c` 中)：
- `arch_tensor_create_random()` (行 35-52)
- `microarch_tensor_create_random()` (行 139-156)
- `microarch_tensor_create_from_desc()` (行 158-176)
- `arch_tensor_gen_sub_tensor()` (行 65-85)
- `microarch_tensor_gen_sub_tensor()` (行 184-204)
- `microarch_tensor_print()` (行 259-269)
- 以及它们依赖的静态函数：
  - `gen_random_data()` (行 10-13)
  - `max_val()` (行 15-17)
  - `compute_arch_strides()` (行 19-24)
  - `get_skip_addr()` (行 26-30)

## 重构方案

### 1. 创建 tests/utils.h

声明测试工具函数：

```c
#ifndef UTILS_H
#define UTILS_H

#include "tensor_descriptor.h"

// 创建函数
arch_tensor_t* arch_tensor_create_random(void);
microarch_tensor_t* microarch_tensor_create_random(void);
microarch_tensor_t* microarch_tensor_create_from_desc(const microarch_tensor_descriptor_t* desc);

// 子张量生成
void arch_tensor_gen_sub_tensor(arch_tensor_t* tensor);
void microarch_tensor_gen_sub_tensor(microarch_tensor_t* tensor);

// 打印
void microarch_tensor_print(const char* name, const microarch_tensor_descriptor_t* desc);

// 销毁
void arch_tensor_destroy(arch_tensor_t* tensor);
void microarch_tensor_destroy(microarch_tensor_t* tensor);

#endif
```

### 2. 创建 tests/utils.c

从 `tensor_descriptor.c` 移动上述静态和公共函数：

```c
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// 静态 helper 函数
static unsigned int gen_random_data(unsigned int max_value) { ... }
static unsigned int max_val(unsigned int x, unsigned int y) { ... }
static void compute_arch_strides(const unsigned int* dim, unsigned int* stride) { ... }
static int get_skip_addr(int base_addr, int skip) { ... }

// 公共函数实现
arch_tensor_t* arch_tensor_create_random(void) { ... }
microarch_tensor_t* microarch_tensor_create_random(void) { ... }
microarch_tensor_t* microarch_tensor_create_from_desc(...) { ... }
void arch_tensor_destroy(arch_tensor_t* tensor) { ... }
void microarch_tensor_destroy(microarch_tensor_t* tensor) { ... }
void arch_tensor_gen_sub_tensor(arch_tensor_t* tensor) { ... }
void microarch_tensor_gen_sub_tensor(microarch_tensor_t* tensor) { ... }
void microarch_tensor_print(const char* name, const microarch_tensor_descriptor_t* desc) { ... }
```

### 3. 更新 src/tensor_descriptor.c

移除已移动的函数（保留核心库函数）：
- 移除 `arch_tensor_create_random()`
- 移除 `microarch_tensor_create_random()`
- 移除 `microarch_tensor_create_from_desc()`
- 移除 `arch_tensor_destroy()`（移动到 utils）
- 移除 `microarch_tensor_destroy()`（移动到 utils）
- 移除 `arch_tensor_gen_sub_tensor()`
- 移除 `microarch_tensor_gen_sub_tensor()`
- 移除 `microarch_tensor_print()`
- 移除所有静态 helper 函数
- 保留核心库函数：`tensor_descriptor_convert()`, `tensor_descriptor_convert_sub()`, `microarch_constraints_convert()`, 约束转换相关函数

### 4. 更新 CMakeLists.txt

添加 `tests/utils.c` 到构建：

```cmake
add_executable(microarchTensorDescriptor tests/microarch_tool.c tests/utils.c)
add_executable(archTensorDescriptor tests/arch_tool.c tests/utils.c)
```

或者使用一个单独的测试可执行文件：

```cmake
add_executable(tensorTest tests/tensor_test.c tests/utils.c)
```

### 5. 头文件调整

由于 `arch_tensor_destroy()` 和 `microarch_tensor_destroy()` 移动到 `tests/utils.h`，需要决定：
- 选项 A：保留 `tensor_descriptor.h` 中的声明，但实现移到 `tests/utils.c`（需要弱符号或链接器技巧）
- 选项 B：只保留 `tests/utils.h` 中的声明，测试代码包含 `utils.h`

推荐选项 B，简单直接。

## 依赖关系

`tests/utils.c` 依赖 `src/tensor_descriptor.h` 中的类型定义：
- `arch_tensor_t`
- `microarch_tensor_t`
- `microarch_tensor_descriptor_t`
- `sub_arch_tensor_descriptor_t`

这些类型保持不变。

## 测试策略

每完成一个任务后运行构建验证：
```bash
cd build && cmake .. && make
```

确保所有测试文件能正确编译链接。