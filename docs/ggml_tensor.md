# GGML Tensor Structure Documentation

## Overview

`struct ggml_tensor` is the core data structure in the GGML (Georgi Gerganov Machine Learning) tensor library. It represents a multi-dimensional tensor with support for various data types, non-contiguous memory layouts, and computational graph operations.

## Structure Definition

```c
#define GGML_MAX_DIMS 4

struct ggml_tensor {
    enum ggml_type type;                           // Data type (F32, F16, Q4_0, etc.)
    struct ggml_backend_buffer * buffer;           // Memory buffer
    
    int64_t ne[GGML_MAX_DIMS];                     // Number of elements in each dimension
    size_t  nb[GGML_MAX_DIMS];                     // Stride in bytes for each dimension
    
    enum ggml_op op;                               // Tensor operation
    int32_t op_params[GGML_MAX_OP_PARAMS / sizeof(int32_t)];  // Operation parameters
    int32_t flags;                                 // Tensor flags
    
    struct ggml_tensor * src[GGML_MAX_SRC];        // Source tensors (up to 10)
    struct ggml_tensor * view_src;                 // Source tensor for views
    size_t               view_offs;                // View offset
    
    void * data;                                   // Pointer to tensor data
    char name[GGML_MAX_NAME];                      // Tensor name (up to 64 chars)
    void * extra;                                  // Backend-specific data
    char padding[8];                               // Padding for alignment
};
```

## Key Fields Explained

### 1. `ne[GGML_MAX_DIMS]` - Number of Elements

`ne` (number of elements) is an array of 4 int64_t values representing the size of each dimension:

- `ne[0]`: Size of dimension 0 (columns/features)
- `ne[1]`: Size of dimension 1 (rows/sequences)
- `ne[2]`: Size of dimension 2 (heads/layers)
- `ne[3]`: Size of dimension 3 (batches)

**Example:**
- 1D tensor with 1024 elements: `ne = {1024, 1, 1, 1}`
- 2D matrix 512x768: `ne = {512, 768, 1, 1}`
- 3D tensor 64x64x3: `ne = {64, 64, 3, 1}`

### 2. `nb[GGML_MAX_DIMS]` - Stride in Bytes

`nb` (stride in bytes) defines how many bytes to skip to move to the next element along each dimension.

#### The nb[1] Formula Explained

```c
// For a contiguous tensor:
nb[0] = ggml_type_size(type)                           // Bytes per element
nb[1] = nb[0] * (ne[0] / ggml_blck_size(type))        // Row stride ← Why division?
nb[i] = nb[i-1] * ne[i-1]                             // General formula
```

**Why Division in nb[1]?**

For **quantized types**, data is stored in **blocks**, not individual elements. The division calculates how many blocks are needed:

```
nb[1] = (number of blocks in row) × (bytes per block)
      = (ne[0] / blck_size) × type_size
```

#### Example: F32 vs Q4_0 Comparison

**F32 (Non-quantized)**
```c
ne[0] = 128
blck_size(F32) = 1      // Each element is its own "block"
type_size(F32) = 4      // 4 bytes per element

nb[1] = (128 / 1) × 4 = 512 bytes
// 128 elements × 4 bytes each = 512 bytes per row
```

**Q4_0 (Quantized)**
```c
ne[0] = 128
blck_size(Q4_0) = 32    // 32 elements per block
type_size(Q4_0) = 18    // 18 bytes per block (4-bit weights + scales)

nb[1] = (128 / 32) × 18 = 4 × 18 = 72 bytes
// 4 blocks × 18 bytes each = 72 bytes per row
```

#### Memory Layout Visualization

**F32 Layout (128 elements = 512 bytes):**
```
Memory (linear):
[0] [1] [2] ... [127]
└── 128 elements × 4 bytes = 512 bytes ──┘

Layout: Each element stored separately
Bytes: 0-3, 4-7, 8-11, ... 508-511
```

**Q4_0 Layout (128 elements = 72 bytes):**
```
Memory (linear):
[Block 0: elements 0-31]  [Block 1: elements 32-63]  [Block 2: elements 64-95]  [Block 3: elements 96-127]
└────── 18 bytes ──────┘  └────── 18 bytes ──────┘  └────── 18 bytes ──────┘  └────── 18 bytes ──────┘
└────────────────────────────────── 72 bytes total ──────────────────────────────────┘

Block structure (per 32 elements):
- Scale: 2 bytes (F16)
- Weights: 16 bytes (4 bits × 32 elements = 128 bits = 16 bytes)
- Total: 18 bytes per block
```

#### Formula Breakdown Table

| Step | Operation | F32 | Q4_0 | Meaning |
|------|-----------|-----|------|---------|
| 1 | `ne[0]` | 128 | 128 | Elements per row |
| 2 | `blck_size` | 1 | 32 | Elements per block |
| 3 | `ne[0] / blck_size` | 128 | 4 | **Blocks needed** ← Division! |
| 4 | `type_size` | 4 | 18 | Bytes per block |
| 5 | `nb[1] = (3) × (4)` | 512 | 72 | **Total row bytes** |

**Key Insight:** Division is required because quantized data is packed into blocks. We need to count blocks first, then multiply by block size.

#### Key Properties
- `nb[0]`: Bytes to skip to next element in dimension 0 (element size)
- `nb[1]`: Bytes to skip to next element in dimension 1 (row stride)
- `nb[2]`: Bytes to skip to next element in dimension 2 (plane stride)
- `nb[3]`: Bytes to skip to next element in dimension 3 (volume stride)

### 3. Data Type and Block Size

Different types have different block sizes and element sizes:

| Type | Block Size | Bytes per Block | Use Case |
|------|-----------|-----------------|----------|
| F32  | 1         | 4               | Full precision |
| F16  | 1         | 2               | Half precision |
| Q4_0 | 32        | 18              | 4-bit quantization |
| Q8_0 | 32        | 34              | 8-bit quantization |

**Formulas:**
```c
// For quantized types
bytes_per_element = ggml_type_size(type) / ggml_blck_size(type)

// Total bytes for a row
row_bytes = nb[1] = nb[0] * ceil(ne[0] / block_size)
```

## Memory Layout

### Contiguous Tensor (Standard Layout)

```
For a 2D tensor with ne = {4, 3}, type = F32:

Memory Layout (row-major):
[0,0] [1,0] [2,0] [3,0] | [0,1] [1,1] [2,1] [3,1] | [0,2] [1,2] [2,2] [3,2]

ne = {4, 3, 1, 1}
nb = {4, 16, 48, 48}  // sizeof(float)=4, nb[1]=4*4=16, nb[2]=16*3=48

Element access formula:
element(x, y) = data + y * nb[1] + x * nb[0]
```

### Non-Contiguous Tensor (Transposed)

```
For a transposed 2D tensor:

Original: ne = {4, 3}, nb = {4, 16, 48, 48}
Transposed: ne = {3, 4}, nb = {16, 4, 48, 48}

Memory remains the same, but indexing changes:
element(x, y) = data + y * nb[1] + x * nb[0]
              = data + y * 4 + x * 16
```

### Strided Tensor (View with Padding)

```
For a tensor with padding between rows:
ne = {4, 3, 1, 1}
nb = {4, 32, 96, 96}  // Row stride is 32 instead of 16

Memory Layout:
[0,0] [1,0] [2,0] [3,0] [pad]...[pad] | [0,1] [1,1] [2,1] [3,1] [pad]...[pad] |
```

## Computing Strides

### For Contiguous Tensors

```c
// Standard contiguous stride calculation
nb[0] = ggml_type_size(type);
for (int i = 1; i < GGML_MAX_DIMS; i++) {
    nb[i] = nb[i-1] * ((ne[i-1] + blck_size - 1) / blck_size) * blck_size;
    // Or simply for non-quantized: nb[i] = nb[i-1] * ne[i-1]
}
```

### For Non-Contiguous Tensors

Non-contiguous tensors occur in operations like:
- **Transpose**: Swap ne[i] and ne[j], swap nb[i] and nb[j]
- **Permute**: Reorder dimensions, strides follow suit
- **Slice/View**: Different ne with same or adjusted nb
- **Broadcasting**: ne=1 but nb=0 (repeated elements)

## Tensor Operations and Graph

### Source Tensors

```c
struct ggml_tensor * src[GGML_MAX_SRC];  // Up to 10 source tensors
```

Each tensor operation (add, mul, matmul, etc.) creates a new tensor that references its inputs via `src`.

### Example: Addition

```c
struct ggml_tensor * c = ggml_add(ctx, a, b);
// c->src[0] == a
// c->src[1] == b
// c->op == GGML_OP_ADD
```

## Data Access Patterns

### Iterating Over Elements

```c
// Safe element access for 2D tensor
for (int y = 0; y < ne[1]; y++) {
    for (int x = 0; x < ne[0]; x++) {
        void * ptr = (char*)data + y * nb[1] + x * nb[0];
        float value = *(float*)ptr;  // For F32 type
    }
}
```

### Checking Contiguity

```c
bool is_contiguous(const struct ggml_tensor * tensor) {
    size_t expected_nb = ggml_type_size(tensor->type);
    for (int i = 0; i < GGML_MAX_DIMS; i++) {
        if (tensor->nb[i] != expected_nb) return false;
        expected_nb *= tensor->ne[i];
    }
    return true;
}
```

## Common Patterns in LLMs

### Weight Matrices

```
Linear layer weights: ne = {in_features, out_features, 1, 1}
For a 4096x4096 matrix with Q4_0 quantization:
ne = {4096, 4096, 1, 1}
nb = {18, 2304, 9437184, 9437184}  // 4096/32*18 = 2304 bytes per row
```

### Attention Tensors

```
Query/Key/Value: ne = {head_dim, num_heads, seq_len, batch}
For 32 heads, 128 dim, 2048 seq, batch 1:
ne = {128, 32, 2048, 1}
nb = {4, 512, 16384, 33554432}  // F32 type
```

### KV Cache

```
KV cache tensor: ne = {head_dim, num_heads, seq_len, 2}
For caching key and value:
ne = {128, 32, 8192, 2}
nb = {4, 512, 16384, 134217728}
```

## GGUF Format Integration

In GGUF files, tensor metadata includes:
- `tensor.name`: Identifier string
- `tensor.ne`: Dimensions
- `tensor.type`: Quantization type
- Data offset in file

The `nb` values are computed at load time based on type and dimensions.

## Best Practices

1. **Always use nb for indexing**: Never assume contiguous memory
2. **Check tensor flags**: Use `ggml_is_contiguous()` when optimizations are possible
3. **Respect quantization**: Account for block sizes in calculations
4. **Views are lightweight**: Use views instead of copies when possible
5. **Name tensors**: Helps debugging and logging

## References

- [GGML Header](ggml/include/ggml.h)
- [GGUF Specification](https://github.com/ggml-org/ggml/blob/master/docs/gguf.md)
- [LLaMA.cpp Source](src/llama-model.cpp)
