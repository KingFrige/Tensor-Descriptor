/* GGML Tensor Traversal Implementation
 * 
 * Independent implementation of GGML tensor traversal.
 * No dependency on llama.cpp source code.
 */

#include "ggml_tensor.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

/* ============================================================================
 * Type Information Tables
 * 
 * These values are manually extracted from llama.cpp:
 * - ggml_type_size: bytes per block (quantized) or per element (non-quantized)
 * - ggml_blck_size: elements per block (1 for non-quantized types)
 * 
 * Verified against ggml-common.h static assertions:
 * - sizeof(block_q4_0) = sizeof(ggml_half) + QK4_0/2 = 2 + 16 = 18
 * - sizeof(block_q8_0) = sizeof(ggml_half) + QK8_0   = 2 + 32 = 34
 * ============================================================================ */

static const int type_size_table[GGML_TYPE_COUNT] = {
    [GGML_TYPE_F32]  = 4,    /* 4 bytes per float */
    [GGML_TYPE_F16]  = 2,    /* 2 bytes per half */
    [GGML_TYPE_Q4_0] = 18,   /* block_q4_0: 2 (d) + 16 (qs) */
    [GGML_TYPE_Q4_1] = 20,   /* block_q4_1: 4 (dm) + 16 (qs) */
    [GGML_TYPE_Q5_0] = 22,   /* block_q5_0: 2 (d) + 4 (qh) + 16 (qs) */
    [GGML_TYPE_Q5_1] = 24,   /* block_q5_1: 4 (dm) + 4 (qh) + 16 (qs) */
    [GGML_TYPE_Q8_0] = 34,   /* block_q8_0: 2 (d) + 32 (qs) */
    [GGML_TYPE_Q8_1] = 36,   /* block_q8_1: 4 (ds) + 32 (qs) */
};

static const int blck_size_table[GGML_TYPE_COUNT] = {
    [GGML_TYPE_F32]  = 1,    /* No quantization */
    [GGML_TYPE_F16]  = 1,    /* No quantization */
    [GGML_TYPE_Q4_0] = 32,   /* QK4_0 = 32 elements per block */
    [GGML_TYPE_Q4_1] = 32,   /* QK4_1 = 32 elements per block */
    [GGML_TYPE_Q5_0] = 32,   /* QK5_0 = 32 elements per block */
    [GGML_TYPE_Q5_1] = 32,   /* QK5_1 = 32 elements per block */
    [GGML_TYPE_Q8_0] = 32,   /* QK8_0 = 32 elements per block */
    [GGML_TYPE_Q8_1] = 32,   /* QK8_1 = 32 elements per block */
};

/* ============================================================================
 * Type Information Functions
 * ============================================================================ */

int ggml_type_get_size(int type) {
    if (type < 0 || type >= GGML_TYPE_COUNT) {
        return 0;
    }
    return type_size_table[type];
}

int ggml_type_get_blck_size(int type) {
    if (type < 0 || type >= GGML_TYPE_COUNT) {
        return 0;
    }
    return blck_size_table[type];
}

/* ============================================================================
 * Traversal Functions
 * ============================================================================ */

int ggml_tensor_get_traversal_count(const ggml_tensor_desc_t* desc) {
    if (!desc || desc->blck_size <= 0) {
        return 0;
    }
    
    /* Calculate blocks in dimension 0 (elements / block_size) */
    int64_t blocks_d0 = (desc->ne[0] + desc->blck_size - 1) / desc->blck_size;
    
    /* Total blocks = blocks_d0 * ne[1] * ne[2] * ne[3] */
    int64_t total = blocks_d0 * desc->ne[1] * desc->ne[2] * desc->ne[3];
    
    /* Safety check for overflow */
    if (total > INT_MAX) {
        fprintf(stderr, "Error: traversal count exceeds INT_MAX\n");
        return 0;
    }
    
    return (int)total;
}

ggml_traversal_item_t* ggml_tensor_traversal(const ggml_tensor_desc_t* desc, int* count) {
    if (!desc || !count) {
        if (count) *count = 0;
        return NULL;
    }
    
    /* Calculate total number of blocks */
    int total = ggml_tensor_get_traversal_count(desc);
    if (total <= 0) {
        *count = 0;
        return NULL;
    }
    
    /* Allocate result array */
    ggml_traversal_item_t* items = (ggml_traversal_item_t*)malloc(
        total * sizeof(ggml_traversal_item_t));
    if (!items) {
        fprintf(stderr, "Error: failed to allocate traversal items\n");
        *count = 0;
        return NULL;
    }
    
    /* Calculate blocks in dimension 0 */
    int64_t blocks_d0 = (desc->ne[0] + desc->blck_size - 1) / desc->blck_size;
    
    /* 4D nested loop over blocks with verbose output */
    int idx = 0;
    int elem_idx = 0;
    
    for (int64_t i3 = 0; i3 < desc->ne[3]; i3++) {
        printf("i3Cnt= %ld\n", i3);
        
        for (int64_t i2 = 0; i2 < desc->ne[2]; i2++) {
            printf("  i2Cnt = %ld\n", i2);
            
            for (int64_t i1 = 0; i1 < desc->ne[1]; i1++) {
                printf("    i1Cnt = %ld\n", i1);
                
                for (int64_t i0 = 0; i0 < blocks_d0; i0++) {
                    /* Calculate block address using stride formula:
                     * addr = data + i0*nb[0] + i1*nb[1] + i2*nb[2] + i3*nb[3]
                     */
                    char* addr = (char*)desc->data 
                               + i0 * desc->nb[0]
                               + i1 * desc->nb[1]
                               + i2 * desc->nb[2]
                               + i3 * desc->nb[3];
                    
                    /* Populate traversal item */
                    items[idx].block_idx = idx;
                    items[idx].block_addr = addr;
                    items[idx].elem_start = elem_idx;
                    items[idx].elem_count = desc->blck_size;
                    
                    /* Handle last block partial elements */
                    if (i0 == blocks_d0 - 1) {
                        int64_t remaining = desc->ne[0] - (i0 * desc->blck_size);
                        if (remaining < desc->blck_size) {
                            items[idx].elem_count = (int)remaining;
                        }
                    }
                    
                    /* Print detailed info like microarch_tensor_traversal */
                    printf("      i0Cnt = %4ld, i1Cnt = %ld, i2Cnt = %ld, i3Cnt= %ld, "
                           "idx = %8d, blockAddr = %p, elem_start = %d\n",
                           i0, i1, i2, i3, idx, (void*)addr, elem_idx);
                    
                    idx++;
                    elem_idx += desc->blck_size;
                }
                printf("    ***** i1 done ******\n\n");
                elem_idx = (int)(i1 + 1) * (int)desc->ne[0]; /* Reset for next row */
            }
            printf("  ----- i2 done ------\n\n");
            elem_idx = 0; /* Reset for next plane */
        }
        printf("===== i3 done ======\n\n");
    }
    
    *count = idx;
    return items;
}
