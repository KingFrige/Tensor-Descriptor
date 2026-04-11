/* GGML Tensor Traversal Test
 * 
 * Tests the ggml_tensor_traversal functionality using hardcoded data
 * extracted from qwen_tensor.json - All 102 tensors
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ggml_tensor.h"
#include "ggml_tensor_test_data.inc"

/* Allocate dummy data buffer for testing */
static char* allocate_dummy_data(size_t size) {
    char* data = (char*)malloc(size);
    if (data) {
        memset(data, 0, size);
    }
    return data;
}

/* Test type information functions */
void test_type_info(void) {
    printf("=== Testing Type Information ===\n\n");
    
    struct {
        int type;
        const char* name;
        int expected_size;
        int expected_blck;
    } test_cases[] = {
        {GGML_TYPE_F32,  "F32",  4,  1},
        {GGML_TYPE_F16,  "F16",  2,  1},
        {GGML_TYPE_Q4_0, "Q4_0", 18, 32},
        {GGML_TYPE_Q8_0, "Q8_0", 34, 32},
    };
    
    int passed = 0;
    for (int i = 0; i < 4; i++) {
        int size = ggml_type_get_size(test_cases[i].type);
        int blck = ggml_type_get_blck_size(test_cases[i].type);
        
        int ok = (size == test_cases[i].expected_size && 
                  blck == test_cases[i].expected_blck);
        
        printf("  %s: type_size=%d (expected %d), blck_size=%d (expected %d) %s\n",
               test_cases[i].name,
               size, test_cases[i].expected_size,
               blck, test_cases[i].expected_blck,
               ok ? "[PASS]" : "[FAIL]");
        
        if (ok) passed++;
    }
    
    printf("\nType info tests: %d/4 passed\n\n", passed);
}

/* Test traversal count calculation */
void test_traversal_count(void) {
    printf("=== Testing Traversal Count ===\n\n");
    
    /* Test case 1: F32 vector */
    ggml_tensor_desc_t desc1 = {
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = GGML_TYPE_F32,
        .type_size = 4,
        .blck_size = 1,
        .data = NULL
    };
    int count1 = ggml_tensor_get_traversal_count(&desc1);
    printf("  F32 vector [4096,1,1,1]: count=%d (expected 4096) %s\n",
           count1, count1 == 4096 ? "[PASS]" : "[FAIL]");
    
    /* Test case 2: Q4_0 matrix */
    ggml_tensor_desc_t desc2 = {
        .ne = {4096, 1024, 1, 1},
        .nb = {18, 2304, 2359296, 2359296},
        .type = GGML_TYPE_Q4_0,
        .type_size = 18,
        .blck_size = 32,
        .data = NULL
    };
    int count2 = ggml_tensor_get_traversal_count(&desc2);
    int expected2 = (4096/32) * 1024;  /* 128 * 1024 = 131072 */
    printf("  Q4_0 matrix [4096,1024,1,1]: count=%d (expected %d) %s\n",
           count2, expected2, count2 == expected2 ? "[PASS]" : "[FAIL]");
    
    /* Test case 3: Invalid descriptor */
    ggml_tensor_desc_t desc3 = {
        .ne = {0, 0, 0, 0},
        .blck_size = 0
    };
    int count3 = ggml_tensor_get_traversal_count(&desc3);
    printf("  Invalid descriptor: count=%d (expected 0) %s\n\n",
           count3, count3 == 0 ? "[PASS]" : "[FAIL]");
}

/* Test traversal on a single tensor by index */
void test_single_tensor(int idx) {
    const ggml_test_tensor_info_t* info = ggml_test_get_info(idx);
    if (!info) return;
    
    ggml_tensor_desc_t desc;
    ggml_test_get_tensor_desc(idx, &desc);
    
    /* Skip very large tensors for quick testing */
    int block_count = ggml_tensor_get_traversal_count(&desc);
    if (block_count > 1000000) {
        printf("  [%2d] %-8s of %-12s: ne=[%4ld,%4ld,%2ld,%2ld] - SKIPPED (too large: %d blocks)\n",
               idx, info->field, info->op,
               desc.ne[0], desc.ne[1], desc.ne[2], desc.ne[3],
               block_count);
        return;
    }
    
    /* Allocate dummy data buffer */
    size_t data_size = desc.nb[3] * desc.ne[3];
    if (data_size == 0) data_size = 1024; /* Minimum size */
    desc.data = allocate_dummy_data(data_size);
    if (!desc.data) {
        printf("  [%2d] Failed to allocate test data\n", idx);
        return;
    }
    
    /* Perform traversal */
    int result_count = 0;
    ggml_traversal_item_t* items = ggml_tensor_traversal(&desc, &result_count);
    
    if (!items) {
        printf("  [%2d] Traversal failed!\n", idx);
        free(desc.data);
        return;
    }
    
    /* Verify first item */
    int pass = (items[0].block_addr == desc.data && items[0].block_idx == 0);
    
    printf("  [%2d] %-8s of %-12s: ne=[%4ld,%4ld,%2ld,%2ld], blocks=%6d %s\n",
           idx, info->field, info->op,
           desc.ne[0], desc.ne[1], desc.ne[2], desc.ne[3],
           result_count, pass ? "[PASS]" : "[FAIL]");
    
    free(items);
    free(desc.data);
}

/* Test traversal on first 20 test tensors */
void test_all_tensors(void) {
    printf("=== Testing First 20 Tensors ===\n\n");
    
    int passed = 0;
    int tested = 0;
    int test_limit = 20;
    
    for (int i = 0; i < GGML_TEST_TENSOR_COUNT && tested < test_limit; i++) {
        const ggml_test_tensor_info_t* info = ggml_test_get_info(i);
        if (!info) continue;
        
        ggml_tensor_desc_t desc;
        ggml_test_get_tensor_desc(i, &desc);
        
        int block_count = ggml_tensor_get_traversal_count(&desc);
        
        /* Skip very large tensors for quick testing */
        if (block_count > 10000000) {
            printf("  [%3d] %-8s of %-12s: SKIPPED (too large: %d blocks)\n",
                   i, info->field, info->op, block_count);
            continue;
        }
        
        tested++;
        
        /* Print tensor descriptor information */
        printf("\n========================================\n");
        printf("Tensor [%d]: %s of %s (node %d)\n", i, info->field, info->op, info->node_id);
        printf("========================================\n");
        printf("  ne[4]       = [%ld, %ld, %ld, %ld]\n", desc.ne[0], desc.ne[1], desc.ne[2], desc.ne[3]);
        printf("  nb[4]       = [%zu, %zu, %zu, %zu]\n", desc.nb[0], desc.nb[1], desc.nb[2], desc.nb[3]);
        printf("  type        = %d (%s)\n", desc.type, 
               desc.type == 0 ? "F32" : desc.type == 1 ? "F16" : desc.type == 2 ? "Q4_0" : "OTHER");
        printf("  type_size   = %d bytes\n", desc.type_size);
        printf("  blck_size   = %d elements\n", desc.blck_size);
        printf("  block_count = %d\n", block_count);
        printf("----------------------------------------\n");
        
        /* Allocate and test */
        size_t data_size = desc.nb[3] * desc.ne[3];
        if (data_size == 0) data_size = 1024;
        desc.data = allocate_dummy_data(data_size);
        if (!desc.data) continue;
        
        int result_count = 0;
        ggml_traversal_item_t* items = ggml_tensor_traversal(&desc, &result_count);
        
        if (items && result_count == block_count && items[0].block_addr == desc.data) {
            passed++;
        }
        
        free(items);
        free(desc.data);
        
        /* Break after testing 20 tensors */
        if (tested >= test_limit) {
            printf("\n  (Stopped after testing %d tensors)\n", test_limit);
            break;
        }
    }
    
    printf("\n  Tested: %d tensors, Passed: %d/%d\n\n", 
           tested, passed, tested);
}

/* Test with Q4_0 quantized tensor */
void test_q4_0_traversal(void) {
    printf("=== Testing Q4_0 Traversal ===\n\n");
    
    /* Create a smaller Q4_0 tensor for testing */
    ggml_tensor_desc_t desc = {
        .ne = {128, 4, 1, 1},           /* 128 elements, 4 rows */
        .nb = {18, 72, 288, 288},       /* 18 bytes/block, 72 bytes/row */
        .type = GGML_TYPE_Q4_0,
        .type_size = 18,
        .blck_size = 32,
        .data = NULL
    };
    
    printf("  Q4_0 tensor [128,4,1,1] (4 blocks per row, 4 rows)\n");
    
    int count = ggml_tensor_get_traversal_count(&desc);
    printf("  Expected blocks: %d, Got: %d %s\n\n",
           16, count, count == 16 ? "[PASS]" : "[FAIL]");
    
    /* Allocate dummy data */
    desc.data = allocate_dummy_data(288);
    if (!desc.data) {
        printf("  Failed to allocate test data\n");
        return;
    }
    
    int result_count = 0;
    ggml_traversal_item_t* items = ggml_tensor_traversal(&desc, &result_count);
    
    if (items) {
        /* Show first few items */
        printf("  First 8 blocks:\n");
        for (int i = 0; i < 8 && i < result_count; i++) {
            int row = i / 4;
            int blk_in_row = i % 4;
            printf("    [%d] Row %d, Block %d: addr=%p, elem_start=%d\n",
                   i, row, blk_in_row, items[i].block_addr, items[i].elem_start);
        }
        
        /* Verify elem_count is 32 for Q4_0 */
        if (items[0].elem_count == 32) {
            printf("\n    [PASS] Block size correct (32 elements)\n");
        }
        
        free(items);
    }
    
    free(desc.data);
    printf("\n");
}

/* Show summary of all test tensors */
void show_tensor_summary(void) {
    printf("=== Test Tensor Summary (%d tensors) ===\n\n", GGML_TEST_TENSOR_COUNT);
    
    printf("  Idx  Field    Operation      ne[0]   ne[1]   ne[2]  ne[3]  Type  Blocks\n");
    printf("  ---  ------   ---------      -----   -----   -----  -----  ----  ------\n");
    
    for (int i = 0; i < GGML_TEST_TENSOR_COUNT && i < 20; i++) {
        const ggml_test_tensor_info_t* info = ggml_test_get_info(i);
        if (!info) continue;
        
        ggml_tensor_desc_t desc;
        ggml_test_get_tensor_desc(i, &desc);
        
        int blocks = ggml_tensor_get_traversal_count(&desc);
        const char* type_name = desc.type == 0 ? "F32" : 
                               desc.type == 1 ? "F16" :
                               desc.type == 2 ? "Q4_0" : "OTHER";
        
        printf("  %3d  %-8s %-12s %6ld  %6ld  %5ld  %5ld  %-5s %7d\n",
               i, info->field, info->op,
               desc.ne[0], desc.ne[1], desc.ne[2], desc.ne[3],
               type_name, blocks);
    }
    
    if (GGML_TEST_TENSOR_COUNT > 20) {
        printf("  ... (%d more tensors)\n", GGML_TEST_TENSOR_COUNT - 20);
    }
    printf("\n");
}

int main(void) {
    printf("=================================================\n");
    printf("  GGML Tensor Traversal Test Suite\n");
    printf("  Total test tensors: %d\n", GGML_TEST_TENSOR_COUNT);
    printf("=================================================\n\n");
    
    show_tensor_summary();
    test_type_info();
    test_traversal_count();
    test_all_tensors();
    test_q4_0_traversal();
    
    printf("=================================================\n");
    printf("  Test Suite Complete\n");
    printf("=================================================\n");
    
    return 0;
}
