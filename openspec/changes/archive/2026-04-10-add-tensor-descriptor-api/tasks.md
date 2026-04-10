# Tasks

## 1. 创建 tensor_descriptor.h 统一头文件

- [x] 1.1 定义 NDIM, MAX_BYTE_NUM, 错误码等宏
- [x] 1.2 定义模块A类型 (arch_tensor_descriptor_t, sub_arch_tensor_descriptor_t, arch_tensor_t)
- [x] 1.3 定义模块B类型 (microarch_tensor_descriptor_t, microarch_tensor_t, microarch_constraints_t, microarch_physical_limits_t, microarch_conversion_result_t)
- [x] 1.4 定义公开API类型 (tensor_descriptor_t, tensor_conversion_result_t, tensor_constraints_t, tensor_physical_limits_t)
- [x] 1.5 声明内部函数 (arch_*, microarch_*) - static 或不公开
- [x] 1.6 声明公开API (tensor_descriptor_*)

## 2. 创建 tensor_descriptor.c 实现

- [x] 2.1 实现 arch_tensor 模块 (static 函数)
- [x] 2.2 实现 microarch_tensor 模块 (static 函数)
- [x] 2.3 实现公开API tensor_descriptor_convert()
- [x] 2.4 实现类型转换辅助函数

## 3. 删除旧文件

- [x] 3.1 删除 src/arch_tensor.h
- [x] 3.2 删除 src/arch_tensor.c
- [x] 3.3 删除 src/microarch_tensor.h
- [x] 3.4 删除 src/microarch_tensor.c

## 4. 更新构建配置

- [x] 4.1 更新 CMakeLists.txt 编译 tensor_descriptor.c

## 5. 测试验证

- [x] 5.1 更新 tests/constraint_test.c 包含 tensor_descriptor.h
- [x] 5.2 更新 tests/batch_constraint_test.c 包含 tensor_descriptor.h
- [x] 5.3 编译项目
- [x] 5.4 运行测试验证 (16 + 122 = 138 tests passed)

## 6. 文档更新

- [x] 6.1 更新 README.md 或创建 ARCHITECTURE.md
