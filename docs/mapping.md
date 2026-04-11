# GGML Tensor 到 Microarch Tensor 映射方案

## 1. 核心发现：非连续 Stride 实例

### 1.1 qwen_tensor.txt 关键实例分析

#### 实例 1：第 47 行（非连续 Stride）

```
dst   [1, 1024, 1, 1]  [4096, 4, 4096, 4096]
```

**验证计算：**
- `ne = {1, 1024, 1, 1}`
- `nb = {4096, 4, 4096, 4096}`
- 理论连续 `nb[1] = ne[0] * nb[0] = 1 * 4096 = 4096`
- 实际 `nb[1] = 4`

**差距：4096 ≠ 4，非连续！**

**分析：** 这是一个特殊的布局，`nb[0]=4096` 远大于实际元素大小（应该是 4），可能是某种特殊的 view 或 aligned allocation。

#### 实例 2：第 17 行（连续 Stride - 对比）

```
src1  [128, 1, 1, 1]  [4, 512, 512, 512]
```

**验证：**
- `ne = {128, 1, 1, 1}`
- `nb = {4, 512, 512, 512}`
- 理论 `nb[1] = 128 * 4 = 512` ✓
- 实际 `nb[1] = 512`

**结论：连续 stride**

### 1.2 非连续映射的关键问题

对于实例 47 的非连续情况：

```
GGML: nb[1] = 4 (实际行间距)
理论连续: nb[1] = 4096
```

**映射到 Microarch 的选择：**

| 方案 | 策略 | 优点 | 缺点 |
|------|------|------|------|
| A | 强制对齐<br>`sliceSkip = 4096` | 满足硬件对齐 | 与实际数据布局不符，访问错误数据 |
| B | 直接映射<br>`sliceSkip = 4` | 符合实际布局 | 可能不满足硬件对齐要求 |
| C | 数据重组<br>创建新的连续缓冲区 | 标准映射，兼容性好 | 额外内存和复制开销 |

## 2. Skip 计算的两套逻辑

### 2.1 完整张量路径

```c
// microarch_constraints_convert 函数
result->desc.unitSkip  = (int)next_power_of_2(result->desc.byteNum);
result->desc.sliceSkip = result->desc.unitNum * result->desc.unitSkip;
result->desc.planeSkip = result->desc.sliceNum * result->desc.sliceSkip;
result->desc.cubeSkip  = result->desc.planeNum * result->desc.planeSkip;
```

**特点：**
- unitSkip 从 byteNum 计算（next_power_of_2）
- 上层 skip 通过乘法累积

### 2.2 子张量路径

```c
// ggml_tensor_convert_sub_with_constraints 函数
result->desc.unitSkip  = (int)(tensor->tensorDesc.stride[0] * tensor->subTensorDesc.traversalStride[0]);
result->desc.sliceSkip = (int)(tensor->tensorDesc.stride[1] * tensor->subTensorDesc.traversalStride[1]);
result->desc.planeSkip = (int)(tensor->tensorDesc.stride[2] * tensor->subTensorDesc.traversalStride[2]);
result->desc.cubeSkip  = (int)(tensor->tensorDesc.stride[3] * tensor->subTensorDesc.traversalStride[3]);
```

**特点：**
- Skip 直接从父张量的 stride 计算
- 乘以 traversalStride（遍历步长）
- **可能打破 next_power_of_2 约束**

## 3. 待解决问题

### 3.1 硬件 DMA 能力确认

1. **byteNum 最大值 64** 是否可配置？还是硬件固定约束？
2. **Skip 值是否必须 power-of-2**，还是硬件支持任意值？
3. **非连续 stride** 的 DMA 传输效率如何？是否有性能惩罚？

### 3.2 映射策略选择

对于非连续 stride 场景，需要确定：

1. **强制连续化** vs **直接映射** vs **混合策略**？
2. **阈值设定**：数据量、访问频率、硬件能力如何权衡？
3. **hasGap 标志** 的实际作用：是否影响 DMA 行为？

### 3.3 子张量路径约束

1. **traversalStride** 的具体含义和取值范围？
2. **非 power-of-2 skip** 的硬件支持情况？
3. **完整张量 vs 子张量** 路径的选择策略？

## 4. 下一步行动

1. **硬件规格确认**：查阅硬件手册，确认 DMA 的 skip 约束
2. **测试用例设计**：构造非连续 stride 测试，验证映射正确性
3. **性能基准测试**：对比不同映射策略的性能表现
4. **文档完善**：基于验证结果，完善映射方案的文档和代码实现
