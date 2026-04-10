# Tensor-Descriptor

C11 项目，处理张量描述符和地址生成。

## 构建

```bash
cd build && cmake .. && make
```

## 运行测试

| 命令 | 功能 |
|------|------|
| `./build/constraint_test` | 单项约束测试 |
| `./build/batch_constraint_test` | 批量约束测试 |

## 结构

- `src/` - 核心库 (tensor_descriptor.h/c)
- `tests/` - 测试工具 (utils.h/c) + 测试文件
- `CMakeLists.txt` - C11 构建配置

## 依赖

- 无外部库依赖 (使用整型位运算替代 math.h)
- C11 标准

## 注意

1. 小步修改频繁回归测试
2. 结束需要 review，禁止擅自 git commit
3. 禁止修改代码格式，仅修改项目需要的
