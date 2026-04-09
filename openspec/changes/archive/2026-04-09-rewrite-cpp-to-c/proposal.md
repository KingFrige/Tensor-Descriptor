## Why

当前项目使用 C++ 实现（`microarch_tensor.hh`, `arch_tensor.hh`），依赖于 C++ STL 和 nlohmann/json 库。为了提升代码的可移植性、减少依赖、并适配嵌入式或低资源环境，需要将代码重写为纯 C 实现。同时，需要移除对 nlohmann/json 库的依赖，实现自定义的轻量级 JSON 解析器。

## What Changes

- **BREAKING**: 将 `src/microarch_tensor.hh` 从 C++ 类重写为 C 结构体 + 函数
- **BREAKING**: 将 `src/arch_tensor.hh` 从 C++ 类重写为 C 结构体 + 函数
- **BREAKING**: 移除 `<nlohmann/json.hpp>` 依赖
- **REMOVED**: 移除 JSON 解析器和配置文件支持（configs/demo.json 将被删除）
- 更新 CMakeLists.txt，将编译器从 C++ 改为 C
- **更新测试程序（全部使用 C 重写）**:
  - `tests/microarch_main.cc` → `tests/microarch_main.c`
  - `tests/arch_main.cc` → `tests/arch_main.c`
  - `tests/microarch_tool.cc` → `tests/microarch_tool.c`（移除 JSON 解析，使用硬编码配置）
- 保持现有功能和 API 语义不变（功能等价）

## Capabilities

### New Capabilities
- `c-tensor-descriptor`: C 语言实现的张量描述符核心库（包含 microarch 和 arch 两层）

### Removed Capabilities
- JSON 配置文件解析功能
- nlohmann_json 外部依赖

## Impact

- **核心库文件**: `src/microarch_tensor.hh`, `src/arch_tensor.hh` → 重写为 `.h`/`.c` 文件
- **测试文件**: 所有 `tests/` 目录下的 `.cc` 文件重写为 `.c` 文件:
  - `tests/microarch_main.cc` → `tests/microarch_main.c`
  - `tests/arch_main.cc` → `tests/arch_main.c`
  - `tests/microarch_tool.cc` → `tests/microarch_tool.c`
- **构建系统**: CMakeLists.txt → 更新编译标准从 C++17 改为 C11，更新源文件扩展名
- **依赖**: 移除 `nlohmann_json` 外部依赖
- **删除文件**:
  - `configs/demo.json` (不再需要)
  - `ext/nlohmann_json/` 目录 (依赖移除)
  - `openspec/changes/rewrite-cpp-to-c/specs/c-json-parser/` (JSON 规范不再需要)
- **API**: 从 C++ 类接口改为 C 结构体和函数接口
