# 张量描述符约束系统 - 实现总结

## 1. 需求侧约束 (User Constraints)

用户可通过 `microarch_constraints_t` 结构体传入约束条件：

| 字段 | 说明 |
|------|------|
| `maxUnitNum` | 最大单元数 |
| `maxSliceNum` | 最大切片数 |
| `maxPlaneNum` | 最大平面数 |
| `maxCubeNum` | 最大立方体数 |
| `maxTotalBytes` | 最大总字节数 |

### 约束检查逻辑

- 约束检查在 `validate_arch_constraints()` 函数中实现
- 当维度超出约束时返回 `E_OVER_CONSTRAINED` 错误
- 约束验证发生在物理限制检查之后

## 2. 物理最大约束 (Physical Limits)

物理限制通过 `microarch_physical_limits_t` 结构体定义：

| 字段 | 最大值 | 说明 |
|------|--------|------|
| `maxPhysicalByteNum` | 64 | 单个元素最大字节数 |
| `maxPhysicalUnitNum` | 1024 | 最大单元数 |
| `maxPhysicalSliceNum` | 1024 | 最大切片数 |
| `maxPhysicalPlaneNum` | 1024 | 最大平面数 |
| `maxPhysicalCubeNum` | 1024 | 最大立方体数 |

### 物理限制检查

- 在 `check_physical_limits()` 函数中实现
- 返回 `E_PHYSICAL_CONSTRAINT` 错误
- 检查发生在维度扩展之后

## 3. 维度自动扩展 (Dimension Expansion)

当维度超出物理限制时，自动扩展到下一个维度：

### 溢出映射规则：

1. 总量约束与均衡: arch维度乘积 <= micro 维度乘积
1. 约束计算
   - byteNum约束值  = min(需求侧约束的值, 物理约束)
   - unitNum约束值  = min(需求侧约束的值, 物理约束)
   - sliceNum约束值 = min(需求侧约束的值, 物理约束)
   - planeNum约束值 = min(需求侧约束的值, 物理约束)
   - cubeNum约束值  = min(需求侧约束的值, 物理约束)
1. 溢出从低维度开始，算法: 
   - byteNum = byteNum约束值
   - unitNum = min(arch维度乘积 / byteNum, unitNum约束值)
   - sliceNum = min(arch维度乘积/(byteNum*unitNum), sliceNum约束值)
   - planeNum = min(arch维度乘积/(byteNum*unitNum*sliceNum), planeNum约束值)
   - cubeNum = min(arch维度乘积/(byteNum*unitNum*sliceNum), cubeNum约束值)

1. 均衡，出现 micro 维度乘积 / 即arch维度乘积 > 1.5
   查找到维度值 >1 的最后两个维度均衡，在不超过约束的前提下，micro 维度乘积 / 即arch维度乘积 更接近 1

1. 超约束时自动扩展到其他维度， 只有所有维度都填充达到约束，还是无法满足 micro总量 >= arch 总量的约束，才报错
