## Context

当前项目使用 C++ 实现张量描述符，包含两个层次：
- **microarchTensor**: 微架构层张量描述符，直接描述内存布局
- **archTensor**: 架构层张量描述符，提供高层5维张量抽象

现有实现依赖 C++ STL（iostream, sstream, memory 等）和 nlohmann/json 库。目标是将代码重写为 C11 标准，移除所有 C++ 依赖。

### 当前代码结构
- `microarch_tensor.hh`: 包含 `microarchTensor` 类，有随机生成、子张量生成、地址遍历功能
- `arch_tensor.hh`: 包含 `archTensor` 类，有张量转换、子张量转换功能
- `tests/`: 测试程序使用 C++ API

## Goals / Non-Goals

**Goals:**
- 将 `microarchTensor` 从 C++ 类重写为 C 结构体 + 函数 API
- 将 `archTensor` 从 C++ 类重写为 C 结构体 + 函数 API
- 实现轻量级 JSON 解析器替代 nlohmann/json
- 保持现有功能和 API 语义等价
- 更新构建系统使用 C11 标准

**Non-Goals:**
- 不修改张量描述符的内存布局或地址计算逻辑
- 不添加新功能（纯重写）
- 不修改测试用例的预期输出
- 不优化算法复杂度（保持相同实现）

## Decisions

### 1. C API 设计风格: 显式上下文指针
**决策**: 将类转换为结构体 + 显式上下文指针
```c
// C++ 风格
microarchTensor tensor;
tensor.genSubTensor();

// C 风格
microarch_tensor_t* tensor = microarch_tensor_create();
microarch_tensor_gen_sub(tensor);
microarch_tensor_destroy(tensor);
```
**理由**: 保持面向对象的习惯，同时符合 C 语言惯例

### 2. 内存管理: 显式生命周期
**决策**: 所有动态分配的对象需显式创建和销毁
```c
microarch_tensor_t* tensor = microarch_tensor_create_random();  // 动态分配
int* addrs = microarch_tensor_traversal(tensor);  // 返回动态数组
free(addrs);  // 用户负责释放
microarch_tensor_destroy(tensor);  // 销毁对象
```
**理由**: C 语言没有析构函数，必须显式管理内存

### 3. JSON 解析器: 手写递归下降
**决策**: 实现轻量级递归下降 JSON 解析器，仅支持项目所需特性：
- JSON 数组（顶层为数组）
- JSON 对象（键值对）
- 整数和字符串值
- 不需要：浮点数、嵌套数组、转义字符、unicode

**替代方案**: 使用 jansson 或 cJSON 库
**选择理由**: 零外部依赖，代码量小（<500行），满足需求

### 4. 错误处理: 返回码 + 日志
**决策**: 函数返回 `int` 状态码（0=成功，负值=错误），错误信息输出到 stderr
```c
int result = microarch_tensor_init_from_json(tensor, json_str);
if (result != 0) {
    fprintf(stderr, "Failed to parse JSON\n");
    return result;
}
```
**理由**: C 语言标准错误处理方式，简单可移植

### 5. 头文件组织
**决策**: 保持现有的 `.h` 头文件组织，但将 `.hh` 改为 `.h`
```
src/
  ├── microarch_tensor.h   (原 .hh)
  ├── arch_tensor.h        (原 .hh)
  └── json_parser.h        (新增)
```

## Risks / Trade-offs

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 手动 JSON 解析器有 Bug | 高 | 使用 demo.json 作为测试基准，对比解析结果 |
| 内存泄漏 | 中 | 代码审查，使用 valgrind 检测 |
| 地址计算错误 | 高 | 保持相同算法，对比 C++ 和 C 版本输出 |
| 随机数生成不一致 | 低 | 使用相同种子和时间函数，接受行为差异 |

### 技术债务
- 当前 C++ 代码使用 `time(0)` 作为随机种子，每次调用可能相同
- C 版本将保持此行为，作为已知问题

## Migration Plan

1. **创建 C 头文件**: 定义结构体和函数原型
2. **实现 C 源文件**: 移植 C++ 逻辑到 C
3. **实现 JSON 解析器**: 独立模块，可单独测试
4. **更新测试程序**: 使用 C API 重写所有测试文件:
   - `tests/microarch_main.c`: 测试微架构层张量功能
   - `tests/arch_main.c`: 测试架构层张量功能
   - `tests/microarch_tool.c`: 使用新的 JSON 解析器读取配置
5. **更新 CMake**: 改为 C 编译
6. **验证**: 对比 C++ 和 C 版本输出一致性

### 回滚策略
- 保留原 C++ 文件在 `src/cpp_backup/`（提交前移除）
- 如果 C 版本有问题，可快速切回 C++

## Open Questions

1. **随机数生成**: 是否需要更健壮的随机数生成（如使用 `/dev/urandom`）？
   - 当前决定：保持简单，用于测试即可

2. **JSON 解析器范围**: 是否支持浮点数？
   - 当前决定：仅支持整数（所有张量参数都是整数）

3. **内存分配器**: 是否需要支持自定义分配器？
   - 当前决定：使用标准 malloc/free，简单优先

## 测试策略

### 测试文件重写计划

| 原文件 | 新文件 | 功能 | 关键变更 |
|--------|--------|------|----------|
| `tests/microarch_main.cc` | `tests/microarch_main.c` | 微架构层测试 | iostream→stdio, new→malloc, 类→函数 |
| `tests/arch_main.cc` | `tests/arch_main.c` | 架构层测试 | 同上 + 指针管理 |
| `tests/microarch_tool.cc` | `tests/microarch_tool.c` | JSON配置测试 | nlohmann/json→自定义解析器 |

### 验证方法

1. **输出对比**: C 版本输出应与 C++ 版本结构一致
2. **功能等价**: 相同输入产生相同的地址计算结果
3. **内存检查**: 使用 valgrind 确保无内存泄漏
4. **JSON 测试**: 使用 `configs/demo.json` 验证解析器正确性
