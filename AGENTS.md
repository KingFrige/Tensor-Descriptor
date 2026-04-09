# Tensor-Descriptor

C11 项目，处理张量描述符和地址生成。

## 构建

```bash
cd build && cmake .. && make
```

## 运行

| 命令 | 功能 |
|------|------|
| `./build/microarchTensorDescriptor` | 随机生成微架构层张量并遍历 |
| `./build/archTensorDescriptor` | 随机生成架构层张量并遍历 |
| `./build/microarchTool` | 使用硬编码配置测试 |

## 结构

- `src/` - C 头文件 (`.h`) 和源文件 (`.c`)
- `tests/` - 测试入口 (`.c`)
- `CMakeLists.txt` - C11 构建配置