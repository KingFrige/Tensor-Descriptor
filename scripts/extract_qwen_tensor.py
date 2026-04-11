#!/usr/bin/env python3
"""
Extract tensor test data from qwen_tensor.json for GGML traversal testing.

Usage:
    python3 scripts/extract_qwen_tensor.py

Output:
    tests/ggml_tensor_test_data.inc - C header with hardcoded test tensors
"""

import json
import sys
from pathlib import Path

def extract_all_tensors(json_path):
    """Extract ALL tensor metadata from JSON file (src0, src1, dst from all nodes)."""
    with open(json_path, 'r') as f:
        data = json.load(f)
    
    tensors = []
    tensor_id = 0
    
    for node_idx, node in enumerate(data):
        # Extract all tensor fields from each node
        tensor_fields = ['src0', 'src1', 'dst']
        
        for field in tensor_fields:
            tensor_data = node.get(field)
            if not tensor_data:
                continue
            
            # Skip tensors with invalid dimensions
            ne = tensor_data.get('dimensions', [1, 1, 1, 1])
            if not ne or len(ne) != 4 or all(x == 0 for x in ne):
                continue
            
            tensor = {
                'id': tensor_id,
                'name': tensor_data.get('name', f'{field}_{node_idx}'),
                'node_id': node_idx,
                'field': field,
                'op': node.get('op', 'NONE'),
                'ne': ne,
                'nb': tensor_data.get('strides', [0, 0, 0, 0]),
                'type': tensor_data.get('data_type', 0),
                'type_size': tensor_data.get('type_size', 4),
                'nbytes': tensor_data.get('nbytes', 0),
                'is_view': tensor_data.get('is_view', 0),
            }
            
            # Calculate block_size from type (elements per block)
            type_to_blck = {0: 1, 1: 1, 2: 32, 8: 32}  # F32, F16, Q4_0, Q8_0
            tensor['block_size'] = type_to_blck.get(tensor['type'], 1)
            
            tensors.append(tensor)
            tensor_id += 1
    
    return tensors

def generate_c_header(tensors, output_path):
    """Generate C header file with tensor test data as a struct array."""
    lines = [
        "/* Generated test data from qwen_tensor.json */",
        "/* DO NOT EDIT - Run scripts/extract_qwen_tensor.py to regenerate */",
        "",
        "#ifndef GGML_TENSOR_TEST_DATA_H",
        "#define GGML_TENSOR_TEST_DATA_H",
        "",
        "#include \"ggml_tensor.h\"",
        "",
        f"#define GGML_TEST_TENSOR_COUNT {len(tensors)}",
        "",
        "/* Tensor info structure for test data */",
        "typedef struct {",
        "    int         id;           /* Unique tensor ID */",
        "    const char* name;         /* Tensor name */",
        "    int         node_id;      /* Node index in graph */",
        "    const char* field;        /* src0, src1, or dst */",
        "    const char* op;           /* Operation name */",
        "    int64_t     ne[4];        /* Dimensions */",
        "    size_t      nb[4];        /* Strides */",
        "    int         type;         /* GGML type */",
        "    int         type_size;    /* Bytes per block */",
        "    int         blck_size;    /* Elements per block */",
        "    int         nbytes;       /* Total bytes */",
        "    int         is_view;      /* Is view tensor */",
        "} ggml_test_tensor_info_t;",
        ""
    ]
    
    # Generate individual name strings
    for i, t in enumerate(tensors):
        lines.append(f'static const char ggml_test_name_{i}[] = "{t["name"]}";' )
        lines.append(f'static const char ggml_test_field_{i}[] = "{t["field"]}";' )
        lines.append(f'static const char ggml_test_op_{i}[] = "{t["op"]}";')
    
    lines.append("")
    
    # Generate struct array
    lines.append("/* Array of all test tensors */")
    lines.append("static const ggml_test_tensor_info_t ggml_test_tensors[] = {")
    
    for i, t in enumerate(tensors):
        lines.extend([
            "    {",
            f"        .id = {t['id']},",
            f"        .name = ggml_test_name_{i},",
            f"        .node_id = {t['node_id']},",
            f"        .field = ggml_test_field_{i},",
            f"        .op = ggml_test_op_{i},",
            f"        .ne = {{{t['ne'][0]}, {t['ne'][1]}, {t['ne'][2]}, {t['ne'][3]}}},",
            f"        .nb = {{{t['nb'][0]}, {t['nb'][1]}, {t['nb'][2]}, {t['nb'][3]}}},",
            f"        .type = {t['type']},",
            f"        .type_size = {t['type_size']},",
            f"        .blck_size = {t['block_size']},",
            f"        .nbytes = {t['nbytes']},",
            f"        .is_view = {t['is_view']}",
            "    },"
        ])
    
    lines.extend([
        "};",
        "",
        "/* Helper: Get tensor descriptor from test array */",
        "static inline void ggml_test_get_tensor_desc(int idx, ggml_tensor_desc_t* desc) {",
        "    if (idx < 0 || idx >= GGML_TEST_TENSOR_COUNT) {",
        "        desc->ne[0]=1;desc->ne[1]=1;desc->ne[2]=1;desc->ne[3]=1;",
        "        desc->nb[0]=0;desc->nb[1]=0;desc->nb[2]=0;desc->nb[3]=0;",
        "        desc->type = 0; desc->type_size = 4; desc->blck_size = 1;",
        "        desc->data = NULL;",
        "        return;",
        "    }",
        "    const ggml_test_tensor_info_t* info = &ggml_test_tensors[idx];",
        "    for (int i = 0; i < 4; i++) {",
        "        desc->ne[i] = info->ne[i];",
        "        desc->nb[i] = info->nb[i];",
        "    }",
        "    desc->type = info->type;",
        "    desc->type_size = info->type_size;",
        "    desc->blck_size = info->blck_size;",
        "    desc->data = NULL; /* Caller must allocate/provide */",
        "}",
        "",
        "/* Helper: Get tensor info pointer */",
        "static inline const ggml_test_tensor_info_t* ggml_test_get_info(int idx) {",
        "    if (idx < 0 || idx >= GGML_TEST_TENSOR_COUNT) return NULL;",
        "    return &ggml_test_tensors[idx];",
        "}",
        "",
        "#endif /* GGML_TENSOR_TEST_DATA_H */",
        ""
    ])
    
    with open(output_path, 'w') as f:
        f.write('\n'.join(lines))
    
    print(f"Generated {output_path} with {len(tensors)} tensors")

def main():
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    
    json_path = project_root / "docs" / "qwen_tensor.json"
    output_path = project_root / "tests" / "ggml_tensor_test_data.inc"
    
    if not json_path.exists():
        print(f"Error: {json_path} not found", file=sys.stderr)
        sys.exit(1)
    
    tensors = extract_all_tensors(json_path)
    generate_c_header(tensors, output_path)
    
    # Print summary
    print("\nExtracted tensors:")
    print(f"  Total: {len(tensors)} tensors\n")
    
    # Group by operation
    ops = {}
    for t in tensors:
        op = t['op']
        if op not in ops:
            ops[op] = []
        ops[op].append(t)
    
    print("  Breakdown by operation:")
    for op, ts in sorted(ops.items()):
        print(f"    {op}: {len(ts)} tensors")
    
    print("\n  First 10 tensors:")
    for i, t in enumerate(tensors[:10]):
        blks = (t['ne'][0] + t['block_size'] - 1) // t['block_size']
        total_blks = blks * t['ne'][1] * t['ne'][2] * t['ne'][3]
        print(f"    [{i}] {t['field']} of {t['op']} (node {t['node_id']})")
        print(f"        ne=[{t['ne'][0]},{t['ne'][1]},{t['ne'][2]},{t['ne'][3]}], blocks={total_blks}")

if __name__ == "__main__":
    main()
