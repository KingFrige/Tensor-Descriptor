# TODO

## C Rewrite (DONE - 2026-04-09)

- [x] 将 C++ 代码重写为纯 C 实现
- [x] 移除 nlohmann/json 依赖
- [x] 使用硬编码配置替代 JSON 配置文件

## Add Tensor Constraints (DONE - 2026-04-09)

- [x] 添加 `microarch_constraints_t` 结构体支持用户传入约束
- [x] byteNum 硬编码最大值为 64
- [x] 实现约束检查和自动调整逻辑
- [x] 实现间隙检测标志 `hasGap`
- [x] 约束冲突时返回错误码
- [x] 子张量转换复用约束系统
- [x] 添加 `tests/constraint_test.c` 充分测试

## 测试

1. build/ 目录构建
2. 执行生成的文件测试