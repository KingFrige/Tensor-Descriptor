## Context

GGML 张量格式是 llama.cpp 等推理框架使用的张量表示。与 microarch_tensor 不同，GGML 张量具有以下特点：

1. **维度**: 4D (ne[0], ne[1], ne[2], ne[3])
2. **步长**: stride-based (nb[0-3] in bytes)
3. **量化**: 支持多种量化类型（Q4_0, Q8_0 等）
4. **块结构**: 量化类型使用 block 为单位存储

参考数据（来自 docs/qwen_tensor.json）：
```json
{
  "dimensions": [4096, 151936, 1, 1],
  "strides": [18, 2304, 350060544, 350060544],
  "data_type": 2,        // GGML_TYPE_Q4_0
  "type_size": 18,       // sizeof(block_q4_0)
  "block_size": 32       // QK4_0 (elements per block)
}
```

## Goals / Non-Goals

**Goals:**
- 实现独立于 llama.cpp 的 GGML 张量数据类型定义
- 实现块级遍历功能（类似 microarch_tensor_traversal）
- 支持 F32, F16, Q4_0, Q8_0 等常见类型
- 返回 element 和 block 双重信息
- 使用硬编码测试数据（从 JSON 提取）

**Non-Goals:**
- 不实现完整的 GGML 计算图支持
- 不实现量化/反量化功能
- 不修改现有 microarch_tensor 代码
- 不依赖 llama.cpp 头文件

## Decisions

### 1. 数据类型提取策略
**决策**: 手动提取关键类型定义到独立头文件
```c
// src/ggml_tensor.h - 独立定义
typedef enum {
    GGML_TYPE_F32  = 0,
    GGML_TYPE_F16  = 1,
    GGML_TYPE_Q4_0 = 2,
    GGML_TYPE_Q8_0 = 8,
} ggml_type_t;
```
**理由**: 避免依赖 llama.cpp 的复杂头文件结构

### 2. 遍历粒度
**决策**: 按 block 遍历（而非 element）
```c
// 对于 Q4_0: block_size=32, type_size=18
// 遍历 128 个 blocks (4096/32)，而非 4096 个 elements
typedef struct {
    int      block_idx;      // 块索引
    void*    block_addr;     // 块起始地址
    int      elem_start;     // 块内起始元素索引
    int      elem_count;     // 块内元素数量
} ggml_traversal_item_t;
```
**理由**: 与量化数据结构的物理存储对齐

### 3. API 设计
**决策**: 类似 microarch_tensor_traversal 的接口风格
```c
// 获取遍历项数量
int ggml_tensor_get_traversal_count(const ggml_tensor_desc_t* desc);

// 执行遍历，返回数组（需调用者释放）
ggml_traversal_item_t* ggml_tensor_traversal(const ggml_tensor_desc_t* desc, int* count);
```
**理由**: 保持项目内部 API 一致性

### 4. 测试数据策略
**决策**: Python 脚本提取 + C 硬编码
```python
# scripts/extract_qwen_tensor.py
# 读取 docs/qwen_tensor.json
# 生成 tests/ggml_tensor_test_data.inc
```
**理由**: 避免运行时 JSON 解析依赖

### 5. 文件组织
**决策**: 完全独立的文件结构
```
src/
  ├── ggml_tensor.h        # 数据类型定义
  └── ggml_tensor.c        # 遍历实现
tests/
  └── ggml_tensor_traversal_test.c  # 独立测试
scripts/
  └── extract_qwen_tensor.py        # 数据提取
```
**理由**: 与现有代码零耦合，便于迭代

## Risks / Trade-offs

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 类型定义与 llama.cpp 不一致 | 高 | 仅使用稳定的类型编号（F32=0, F16=1, Q4_0=2） |
| 块大小计算错误 | 高 | 参考 ggml-common.h 的静态断言验证 |
| 遍历性能问题 | 低 | 仅用于测试，非生产代码 |
| 数据提取脚本维护 | 低 | JSON 格式稳定，脚本简单 |

## Migration Plan

1. **创建头文件**: 定义 ggml_tensor_desc_t 和相关类型
2. **实现遍历函数**: 按 4D 嵌套循环实现块级遍历
3. **创建提取脚本**: Python 脚本读取 JSON 生成 C 数组
4. **编写测试程序**: 使用硬编码数据测试遍历功能
5. **验证输出**: 对比遍历结果与预期地址计算

## Open Questions

1. **支持的类型范围**: 是否只需要 F32/F16/Q4_0/Q8_0？
   - 当前决定：先支持这 4 种最常见的类型

2. **地址输出格式**: 使用绝对地址还是相对偏移？
   - 当前决定：使用绝对地址（void*），与 microarch_tensor 一致

3. **是否需要 view 支持**: qwen_tensor.json 中有 is_view 标记
   - 当前决定：第一阶段先支持非 view 张量
