#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tensor_descriptor.h"
#include "utils.h"

static int test_count = 0;
static int pass_count = 0;

#define ASSERT(cond, msg) do { \
    test_count++; \
    if (cond) { \
        printf("[PASS] %s\n", msg); \
        pass_count++; \
    } else { \
        printf("[FAIL] %s\n", msg); \
    } \
} while(0)

static void print_conversion_result(const tensor_descriptor_t* desc,
                                    const tensor_conversion_result_t* result) {
    printf("    desc: dim[%u,%u,%u,%u,%u]\n", 
           desc->dimension[0], desc->dimension[1],
           desc->dimension[2], desc->dimension[3],
           desc->dimension[4]);
    printf("    stride: [%u,%u,%u,%u,%u]\n",
           desc->stride[0], desc->stride[1],
           desc->stride[2], desc->stride[3],
           desc->stride[4]);
    if (result->effectiveMaxByte > 0 || result->effectiveMaxUnit > 0 || 
        result->effectiveMaxSlice > 0 || result->effectiveMaxPlane > 0 || 
        result->effectiveMaxCube > 0) {
        printf("    effectiveLimits: maxByte=%u, maxUnit=%u, maxSlice=%u, maxPlane=%u, maxCube=%u\n",
               result->effectiveMaxByte, result->effectiveMaxUnit,
               result->effectiveMaxSlice, result->effectiveMaxPlane,
               result->effectiveMaxCube);
    }
    printf("    micro: byteNum=%d, unitNum=%d, sliceNum=%d, planeNum=%d, cubeNum=%d\n",
           result->desc.byteNum, result->desc.unitNum, result->desc.sliceNum,
           result->desc.planeNum, result->desc.cubeNum);
    if (result->desc.byteNum > 0 && result->desc.sliceNum > 0 && 
        result->desc.planeNum > 0 && result->desc.cubeNum > 0) {
        printf("    micro skip: [unitSkip=%d, sliceSkip=%d, planeSkip=%d, cubeSkip=%d]\n",
               result->desc.unitSkip, result->desc.sliceSkip,
               result->desc.planeSkip, result->desc.cubeSkip);
    }
    printf("    errorCode: %d\n", result->errorCode);
}

void test_no_constraints() {
    printf("\n=== Test: No constraints ===\n");
    
    tensor_descriptor_t desc;
    desc.baseAddr = 5120;
    desc.dimension[0] = 48;
    desc.dimension[1] = 1024;
    desc.dimension[2] = 32;
    desc.dimension[3] = 1;
    desc.dimension[4] = 1;
    desc.stride[0] = 64;
    desc.stride[1] = 65536;
    desc.stride[2] = 2097152;
    desc.stride[3] = 2097152;
    desc.stride[4] = 2097152;
    
    tensor_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = tensor_descriptor_convert(&desc, NULL, NULL, &result);
    
    print_conversion_result(&desc, &result);
    microarch_tensor_traversal((const microarch_tensor_descriptor_t*)&result.desc);
    
    ASSERT(ret == E_SUCCESS, "Conversion succeeds with no constraints");
    ASSERT(result.errorCode == E_SUCCESS, "Error code is E_SUCCESS");
    
    unsigned long long arch_bytes = (unsigned long long)desc.dimension[0] *
                                    desc.dimension[1] *
                                    desc.dimension[2] *
                                    desc.dimension[3] *
                                    desc.dimension[4];
    unsigned long long micro_bytes = (unsigned long long)result.desc.byteNum *
                                     result.desc.unitNum *
                                     result.desc.sliceNum *
                                     result.desc.planeNum *
                                     result.desc.cubeNum;
    
    ASSERT(arch_bytes == micro_bytes, "Data volume preserved");
    
    printf("arch_bytes=%llu, micro_bytes=%llu\n", arch_bytes, micro_bytes);
}

void test_byte_num_exceeded() {
    printf("\n=== Test: byteNum > 64 auto-expands ===\n");
    
    tensor_descriptor_t desc;
    desc.baseAddr = 5120;
    desc.dimension[0] = 128;
    desc.dimension[1] = 1;
    desc.dimension[2] = 1;
    desc.dimension[3] = 1;
    desc.dimension[4] = 1;
    desc.stride[0] = 128;
    desc.stride[1] = 128;
    desc.stride[2] = 128;
    desc.stride[3] = 128;
    desc.stride[4] = 128;
    
    unsigned long long arch_bytes = 128ULL;
    
    tensor_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = tensor_descriptor_convert(&desc, NULL, NULL, &result);
    
    print_conversion_result(&desc, &result);
    
    ASSERT(ret == E_SUCCESS, "Conversion succeeds (auto-expands dim[0] > 64)");
    ASSERT(result.desc.byteNum == 64, "byteNum clamped to 64");
    ASSERT(result.desc.unitNum >= 2, "unitNum expanded >= 2");
    
    unsigned long long micro_bytes = (unsigned long long)result.desc.byteNum * result.desc.unitNum *
                                     result.desc.sliceNum * result.desc.planeNum * result.desc.cubeNum;
    ASSERT(micro_bytes >= arch_bytes, "Micro bytes >= arch bytes");
}

void test_alignment_gap() {
    printf("\n=== Test: Alignment gap (not applicable with direct mapping) ===\n");
    
    tensor_descriptor_t desc;
    desc.baseAddr = 5120;
    desc.dimension[0] = 33;
    desc.dimension[1] = 1;
    desc.dimension[2] = 1;
    desc.dimension[3] = 1;
    desc.dimension[4] = 1;
    desc.stride[0] = 64;
    desc.stride[1] = 64;
    desc.stride[2] = 64;
    desc.stride[3] = 64;
    desc.stride[4] = 64;
    
    tensor_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = tensor_descriptor_convert(&desc, NULL, NULL, &result);
    
    print_conversion_result(&desc, &result);
    
    ASSERT(ret == E_SUCCESS, "Conversion succeeds");
}

void test_user_constraints() {
    printf("\n=== Test: User constraints ===\n");
    
    tensor_descriptor_t desc;
    desc.baseAddr = 5120;
    desc.dimension[0] = 64;
    desc.dimension[1] = 100;
    desc.dimension[2] = 1;
    desc.dimension[3] = 1;
    desc.dimension[4] = 1;
    desc.stride[0] = 64;
    desc.stride[1] = 6400;
    desc.stride[2] = 6400;
    desc.stride[3] = 6400;
    desc.stride[4] = 6400;
    
    tensor_constraints_t constraints;
    constraints.maxByteNum = 0;
    constraints.maxUnitNum = 50;
    constraints.maxSliceNum = 0;
    constraints.maxPlaneNum = 0;
    constraints.maxCubeNum = 0;
    constraints.maxTotalBytes = 0;
    
    tensor_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = tensor_descriptor_convert(&desc, &constraints, NULL, &result);
    
    print_conversion_result(&desc, &result);
    
    ASSERT(ret == E_SUCCESS, "Conversion succeeds with user constraints");
    ASSERT(result.desc.unitNum <= 50, "unitNum within constraint");
}

void test_physical_limits() {
    printf("\n=== Test: Physical limits ===\n");
    
    tensor_descriptor_t desc;
    desc.baseAddr = 5120;
    desc.dimension[0] = 64;
    desc.dimension[1] = 10000;
    desc.dimension[2] = 1;
    desc.dimension[3] = 1;
    desc.dimension[4] = 1;
    desc.stride[0] = 64;
    desc.stride[1] = 640000;
    desc.stride[2] = 640000;
    desc.stride[3] = 640000;
    desc.stride[4] = 640000;
    
    tensor_physical_limits_t limits;
    limits.maxPhysicalByteNum = 64;
    limits.maxPhysicalUnitNum = 1024;
    limits.maxPhysicalSliceNum = 1024;
    limits.maxPhysicalPlaneNum = 1024;
    limits.maxPhysicalCubeNum = 1024;
    
    tensor_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = tensor_descriptor_convert(&desc, NULL, &limits, &result);
    
    print_conversion_result(&desc, &result);
    
    ASSERT(ret == E_SUCCESS, "Auto-expansion should succeed");
    ASSERT(result.desc.byteNum == 64, "byteNum stays at max");
    ASSERT((unsigned int)result.desc.unitNum <= limits.maxPhysicalUnitNum, "unitNum within limit");
    
    unsigned long long arch_bytes = 64ULL * 10000ULL;
    unsigned long long micro_bytes = (unsigned long long)result.desc.byteNum * 
                                      result.desc.unitNum * 
                                      result.desc.sliceNum * 
                                      result.desc.planeNum * 
                                      result.desc.cubeNum;
    ASSERT(micro_bytes >= arch_bytes, "Micro bytes >= arch bytes");
}

void test_overconstrained() {
    printf("\n=== Test: Over constrained ===\n");
    
    tensor_descriptor_t desc;
    desc.baseAddr = 5120;
    desc.dimension[0] = 64;
    desc.dimension[1] = 100;
    desc.dimension[2] = 1;
    desc.dimension[3] = 1;
    desc.dimension[4] = 1;
    desc.stride[0] = 64;
    desc.stride[1] = 6400;
    desc.stride[2] = 6400;
    desc.stride[3] = 6400;
    desc.stride[4] = 6400;
    
    tensor_constraints_t constraints = {0};
    constraints.maxUnitNum = 10;
    constraints.maxSliceNum = 0;
    constraints.maxPlaneNum = 0;
    constraints.maxCubeNum = 0;
    constraints.maxTotalBytes = 100;
    
    tensor_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = tensor_descriptor_convert(&desc, &constraints, NULL, &result);
    
    print_conversion_result(&desc, &result);
    
    ASSERT(ret == E_OVER_CONSTRAINED, "Should fail with overconstrained error");
    ASSERT(result.errorCode == E_OVER_CONSTRAINED, "Error code is E_OVER_CONSTRAINED");
}

void test_direct_mapping() {
    printf("\n=== Test: Direct Mapping (Q4_0-like tensor) ===\n");
    
    /* Test Q4_0-like tensor: byteNum=18 should map to unitSkip=18 */
    tensor_descriptor_t desc;
    desc.baseAddr = 0;
    desc.dimension[0] = 18;  /* byteNum = 18 (like Q4_0 block size) */
    desc.dimension[1] = 128; /* unitNum = 128 blocks */
    desc.dimension[2] = 1;
    desc.dimension[3] = 1;
    desc.dimension[4] = 1;
    desc.stride[0] = 18;
    desc.stride[1] = 18 * 128;
    desc.stride[2] = 18 * 128;
    desc.stride[3] = 18 * 128;
    desc.stride[4] = 18 * 128;
    
    tensor_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = tensor_descriptor_convert(&desc, NULL, NULL, &result);
    
    print_conversion_result(&desc, &result);
    
    ASSERT(ret == E_SUCCESS, "Direct mapping conversion should succeed");
    ASSERT(result.desc.byteNum == 18, "byteNum should be 18");
    ASSERT(result.desc.unitSkip == 18, "unitSkip should be 18 (direct mapping)");
    
    /* Calculate expected memory usage */
    int expected_memory = result.desc.unitNum * result.desc.unitSkip;
    printf("    Memory usage: %d bytes (optimal with direct mapping)\n", expected_memory);
}

void test_memory_efficiency() {
    printf("\n=== Test: Memory Efficiency Comparison ===\n");
    
    /* Test different byte sizes to verify optimal memory usage */
    int test_sizes[] = {4, 18, 32, 34, 64};
    const char* size_names[] = {"F32", "Q4_0", "Power-of-2", "Q8_0", "Max"};
    
    for (int i = 0; i < 5; i++) {
        tensor_descriptor_t desc;
        desc.baseAddr = 0;
        desc.dimension[0] = test_sizes[i];
        desc.dimension[1] = 100;
        desc.dimension[2] = 1;
        desc.dimension[3] = 1;
        desc.dimension[4] = 1;
        desc.stride[0] = test_sizes[i];
        desc.stride[1] = test_sizes[i] * 100;
        desc.stride[2] = test_sizes[i] * 100;
        desc.stride[3] = test_sizes[i] * 100;
        desc.stride[4] = test_sizes[i] * 100;
        
        tensor_conversion_result_t result;
        memset(&result, 0, sizeof(result));
        
        int ret = tensor_descriptor_convert(&desc, NULL, NULL, &result);
        
        if (ret == E_SUCCESS) {
            int memory = result.desc.unitNum * result.desc.unitSkip;
            int optimal = 100 * test_sizes[i];
            printf("    %s (byteNum=%d): unitSkip=%d, memory=%d bytes (optimal: %d)\n",
                   size_names[i], test_sizes[i], result.desc.unitSkip, memory, optimal);
            ASSERT(result.desc.unitSkip == test_sizes[i], "unitSkip should equal byteNum");
        }
    }
}

int main(void) {
    printf("=== Constraint System Tests ===\n");
    
    test_no_constraints();
    test_byte_num_exceeded();
    test_alignment_gap();
    test_user_constraints();
    test_physical_limits();
    test_overconstrained();
    test_direct_mapping();
    test_memory_efficiency();
    
    printf("\n=== Results: %d/%d passed ===\n", pass_count, test_count);
    
    return (pass_count == test_count) ? 0 : 1;
}
