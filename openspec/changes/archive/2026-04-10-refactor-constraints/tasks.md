# Tasks

## 1. 重构职权分离 (物理约束 vs 需求约束)

- [x] 1.1 分析现有代码结构
- [x] 1.2 物理约束保留在 microarch_tensor.c
- [x] 1.3 需求约束保留在 arch_tensor.c
- [x] 1.4 转换逻辑移到 microarch_tensor.c

## 2. 更新 arch_tensor 调用 microarch 转换

- [x] 2.1 arch_tensor_convert_with_constraints 调用 microarch_constraints_convert
- [x] 2.2 arch_tensor_convert_sub_with_constraints 调用 microarch_constraints_convert
- [x] 2.3 移除 arch_tensor.c 中重复的约束函数

## 3. 测试验证

- [x] 3.1 构建项目 `./build.sh --build`
- [x] 3.2 运行所有测试 `./build.sh -t`
- [x] 3.3 验证 146 个测试全部通过

## 4. 代码清理

- [x] 4.1 移除 arch_tensor.c 中未使用的函数
- [x] 4.2 清理 microarch_tensor.c 中重复代码
