# Tasks

## 1. 创建测试工具模块

- [ ] 1.1 创建 tests/utils.h 声明文件
- [ ] 1.2 创建 tests/utils.c 实现文件
- [ ] 1.3 从 tensor_descriptor.c 复制静态 helper 函数到 utils.c
  - `gen_random_data()`
  - `max_val()`
  - `compute_arch_strides()`
  - `get_skip_addr()`

## 2. 移动测试函数

- [ ] 2.1 移动 arch_tensor_create_random() 到 utils.c
- [ ] 2.2 移动 microarch_tensor_create_random() 到 utils.c
- [ ] 2.3 移动 microarch_tensor_create_from_desc() 到 utils.c
- [ ] 2.4 移动 arch_tensor_destroy() 到 utils.c
- [ ] 2.5 移动 microarch_tensor_destroy() 到 utils.c
- [ ] 2.6 移动 arch_tensor_gen_sub_tensor() 到 utils.c
- [ ] 2.7 移动 microarch_tensor_gen_sub_tensor() 到 utils.c
- [ ] 2.8 移动 microarch_tensor_print() 到 utils.c

## 3. 更新核心库

- [ ] 3.1 从 src/tensor_descriptor.c 移除已移动的函数
- [ ] 3.2 保留核心库函数：tensor_descriptor_convert(), tensor_descriptor_convert_sub(), microarch_constraints_convert()

## 4. 构建验证

- [ ] 4.1 更新 CMakeLists.txt 添加 tests/utils.c
- [ ] 4.2 构建项目 `cd build && cmake .. && make`
- [ ] 4.3 验证构建成功

## 5. 测试验证

- [ ] 5.1 运行现有测试验证功能正常
- [ ] 5.2 确保所有测试通过