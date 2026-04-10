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
    printf("    hasGap: %d, errorCode: %d\n", result->hasGap, result->errorCode);
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
    
    if (result.hasGap == 0) {
        ASSERT(arch_bytes == micro_bytes, "Data volume preserved when hasGap=0");
    }
    
    printf("arch_bytes=%llu, micro_bytes=%llu, hasGap=%d\n", arch_bytes, micro_bytes, result.hasGap);
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

void test_has_gap_detection() {
    printf("\n=== Test: hasGap detection ===\n");
    
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
    ASSERT(result.hasGap == 1, "hasGap = 1 when byteNum = 33 (power of 2 rounding)");
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

int main(void) {
    printf("=== Constraint System Tests ===\n");
    
    test_no_constraints();
    test_byte_num_exceeded();
    test_has_gap_detection();
    test_user_constraints();
    test_physical_limits();
    test_overconstrained();
    
    printf("\n=== Results: %d/%d passed ===\n", pass_count, test_count);
    
    return (pass_count == test_count) ? 0 : 1;
}
