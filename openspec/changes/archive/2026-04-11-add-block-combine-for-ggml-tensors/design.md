# Design: Block Combine for GGML Tensors

## Goals

1. 减少 GGML tensor 到 microarch 映射后的访问次数
2. 保持实现简单（向上取整，无需余数处理）
3. 使用全量测试数据验证（102 tensors）
4. 覆盖所有 GGML 类型组合

## Algorithm

### Block Combine Logic

```c
// 1. 计算原始 blocks
total_blocks = ceil(ne[0] / block_size)

// 2. 确定 target_byteNum（自动优化）
max_bn = 64  // MAX_BYTE_NUM
target_byteNum = floor(max_bn / type_size) * type_size

// 3. 计算组合参数
blocks_per_unit = target_byteNum / type_size
unitNum = ceil(total_blocks / blocks_per_unit)

// 4. 计算 skips
unitSkip = blocks_per_unit * nb[0]
sliceSkip = unitNum * unitSkip
...
```

### Key Design Decisions

| 决策 | 选择 | 理由 |
|------|------|------|
| 余数处理 | 向上取整 | 简单高效，多搬运数据不影响性能 |
| target_byteNum | 自动计算 | 从 64 向下找最大可整除值 |
| 配置方式 | 可选参数 | config=NULL 保持向后兼容 |
| 测试策略 | 全量 102 tensors | 确保所有类型组合正确 |

## GGML Type Coverage

从 `ggml_tensor.h` 需要测试的类型：

| Type | Value | type_size | blck_size | target_byteNum |
|------|-------|-----------|-----------|----------------|
| F32  | 0     | 4         | 1         | 64 (16×4)      |
| F16  | 1     | 2         | 1         | 64 (32×2)      |
| Q4_0 | 2     | 18        | 32        | 54 (3×18)      |
| Q4_1 | 3     | 20        | 32        | 60 (3×20)      |
| Q5_0 | 6     | 22        | 32        | 44 (2×22)      |
| Q5_1 | 7     | 24        | 32        | 48 (2×24)      |
| Q8_0 | 8     | 34        | 32        | 34 (1×34)      |
| Q8_1 | 9     | 36        | 32        | 36 (1×36)      |

## Test Data Source

使用 `tests/ggml_tensor_test_data.inc`：
- 102 个真实 tensor 配置
- 从 `docs/qwen_tensor.json` 提取
- 包含多种类型和维度组合

测试覆盖：
1. 所有类型的 block combine 正确性
2. 访问次数减少验证
3. skip 值计算正确性
4. 边界情况（小 tensor、大 tensor）

## API Changes

```c
// 新增配置结构体
typedef struct {
    int target_byteNum;  // 0=auto, >0=强制指定
    int max_byteNum;     // 上限，默认 64
} ggml_block_combine_config_t;

// 修改函数签名
int ggml_to_microarch_direct_map(
    const int64_t* ne,
    const size_t* nb,
    int type_size,
    int block_size,
    unsigned int baseAddr,
    const ggml_block_combine_config_t* combine_config,  // 新增
    microarch_conversion_result_t* result
);
```

## Files to Modify

1. `src/tensor_descriptor.h` - 添加配置结构体和函数声明
2. `src/tensor_descriptor.c` - 实现 block combine 逻辑
3. `tests/ggml_tensor_traversal_test.c` - 添加全量测试
