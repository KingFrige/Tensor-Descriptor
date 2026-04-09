# Design: 重构约束系统代码

## 目标

清理代码中的重复和硬编码，提高可维护性。

## 问题分析

### 1. 硬编码魔数

在 `arch_tensor.c` 中有多处硬编码 1024：

```
行 250-253: compute_effective_limits()
行 285-288: expand_arch_dimensions()
```

### 2. 代码重复

`compute_effective_limits()` 和 `expand_arch_dimensions()` 包含完全相同的约束合并逻辑。

### 3. Stride 计算重复

在多处出现相同模式：
- `arch_tensor_create_random()` (行 27-31)
- `arch_tensor_convert_to_microarch()` (行 82-85)
- `arch_tensor_convert_with_constraints()` (行 416-419)

### 4. 未使用参数

`adjust_arch_for_constraints()` 的 `totalBytes` 参数从未使用。

## 重构方案

### 1. 提取宏到头文件

```c
// microarch_tensor.h
#define DEFAULT_MAX_UNIT   1024
#define DEFAULT_MAX_SLICE 1024
#define DEFAULT_MAX_PLANE  1024
#define DEFAULT_MAX_CUBE   1024
```

### 2. 提取公共函数

```c
// 合并物理限制和用户约束
static void merge_limits(unsigned int defaults[5],
                        const microarch_physical_limits_t* limits,
                        const microarch_constraints_t* constraints,
                        unsigned int* maxByte,
                        unsigned int* maxUnit,
                        unsigned int* maxSlice,
                        unsigned int* maxPlane,
                        unsigned int* maxCube);
```

### 3. 提取Stride计算

```c
// 计算 stride 数组
static void compute_arch_strides(const unsigned int* dim, unsigned int* stride);
```

### 4. 清理参数

移除 `adjust_arch_for_constraints()` 的 `totalBytes` 参数。

## 测试策略

每完成一个任务后运行：
```bash
./build.sh -a
```

确保所有 146 个测试通过后再进行下一个任务。
