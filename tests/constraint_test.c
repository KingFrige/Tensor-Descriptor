#include "arch_tensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_count = 0;
static int pass_count = 0;

static void print_conversion_result(const arch_tensor_t* tensor,
                                    const microarch_conversion_result_t* result) {
    printf("    arch: dim[%u,%u,%u,%u,%u]\n", 
           tensor->tensorDesc.dimension[0], tensor->tensorDesc.dimension[1],
           tensor->tensorDesc.dimension[2], tensor->tensorDesc.dimension[3],
           tensor->tensorDesc.dimension[4]);
    printf("    arch stride: [%u,%u,%u,%u,%u]\n",
           tensor->tensorDesc.stride[0], tensor->tensorDesc.stride[1],
           tensor->tensorDesc.stride[2], tensor->tensorDesc.stride[3],
           tensor->tensorDesc.stride[4]);
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
    printf("    hasGap: %d\n", result->hasGap);
    if (result->errorCode != E_SUCCESS) {
        const char* errMsg = "UNKNOWN";
        switch (result->errorCode) {
            case E_BYTE_NUM_EXCEEDED: errMsg = "E_BYTE_NUM_EXCEEDED"; break;
            case E_OVER_CONSTRAINED: errMsg = "E_OVER_CONSTRAINED"; break;
            case E_INVALID_DIMENSION: errMsg = "E_INVALID_DIMENSION"; break;
            case E_PHYSICAL_CONSTRAINT: errMsg = "E_PHYSICAL_CONSTRAINT"; break;
        }
        printf("    ERROR: %s (%d)\n", errMsg, result->errorCode);
    }
}

#define ASSERT(cond, msg) do { \
    test_count++; \
    if (cond) { \
        pass_count++; \
        printf("[PASS] %s\n", msg); \
    } else { \
        printf("[FAIL] %s\n", msg); \
    } \
} while(0)

void test_no_constraints() {
    printf("\n=== Test: No constraints ===\n");
    
    arch_tensor_t* tensor = arch_tensor_create_random();
    microarch_conversion_result_t result;
    
    memset(&result, 0, sizeof(result));
    
    int ret = arch_tensor_convert_with_constraints(tensor, NULL, NULL, &result);
    microarch_tensor_traversal(&result.desc);
    int tensorAddrPointNum = microarch_tensor_get_traversal_count(&result.desc);
    printf("tensorAddrPointNum: %d\n", tensorAddrPointNum);
    
    print_conversion_result(tensor, &result);
    
    ASSERT(ret == E_SUCCESS, "Conversion succeeds with no constraints");
    ASSERT(result.errorCode == E_SUCCESS, "Error code is E_SUCCESS");
    
    unsigned long long arch_bytes = (unsigned long long)tensor->tensorDesc.dimension[0] *
                                    tensor->tensorDesc.dimension[1] *
                                    tensor->tensorDesc.dimension[2] *
                                    tensor->tensorDesc.dimension[3] *
                                    tensor->tensorDesc.dimension[4];
    unsigned long long micro_bytes = (unsigned long long)result.desc.byteNum *
                                     result.desc.unitNum *
                                     result.desc.sliceNum *
                                     result.desc.planeNum *
                                     result.desc.cubeNum;
    
    if (result.hasGap == 0) {
        ASSERT(arch_bytes == micro_bytes, "Data volume preserved when hasGap=0");
    }
    
    printf("arch_bytes=%llu, micro_bytes=%llu, hasGap=%d\n", arch_bytes, micro_bytes, result.hasGap);
    
    arch_tensor_destroy(tensor);
}

void test_byte_num_exceeded() {
    printf("\n=== Test: byteNum > 64 auto-expands ===\n");
    
    arch_tensor_t* tensor = arch_tensor_create_random();
    tensor->tensorDesc.dimension[0] = 128;
    tensor->tensorDesc.dimension[1] = 1;
    tensor->tensorDesc.dimension[2] = 1;
    tensor->tensorDesc.dimension[3] = 1;
    tensor->tensorDesc.dimension[4] = 1;
    
    unsigned long long arch_bytes = 128ULL;
    
    microarch_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = arch_tensor_convert_with_constraints(tensor, NULL, NULL, &result);
    
    print_conversion_result(tensor, &result);
    
    ASSERT(ret == E_SUCCESS, "Conversion succeeds (auto-expands dim[0] > 64)");
    ASSERT(result.desc.byteNum == 64, "byteNum clamped to 64");
    ASSERT(result.desc.unitNum >= 2, "unitNum expanded >= 2");
    
    unsigned long long micro_bytes = (unsigned long long)result.desc.byteNum * result.desc.unitNum *
                                     result.desc.sliceNum * result.desc.planeNum * result.desc.cubeNum;
    ASSERT(micro_bytes >= arch_bytes, "Micro bytes >= arch bytes");
    
    arch_tensor_destroy(tensor);
}

void test_has_gap_detection() {
    printf("\n=== Test: hasGap detection ===\n");
    
    arch_tensor_t* tensor = arch_tensor_create_random();
    
    tensor->tensorDesc.dimension[0] = 33;
    microarch_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = arch_tensor_convert_with_constraints(tensor, NULL, NULL, &result);
    
    print_conversion_result(tensor, &result);
    
    ASSERT(ret == E_SUCCESS, "Conversion succeeds");
    ASSERT(result.hasGap == 1, "hasGap=1 when byteNum=33 (stride=64)");
    
    tensor->tensorDesc.dimension[0] = 64;
    memset(&result, 0, sizeof(result));
    ret = arch_tensor_convert_with_constraints(tensor, NULL, NULL, &result);
    
    print_conversion_result(tensor, &result);
    ASSERT(result.hasGap == 0, "hasGap=0 when byteNum=64 (power of 2)");
    
    tensor->tensorDesc.dimension[0] = 32;
    memset(&result, 0, sizeof(result));
    ret = arch_tensor_convert_with_constraints(tensor, NULL, NULL, &result);
    
    print_conversion_result(tensor, &result);
    ASSERT(result.hasGap == 0, "hasGap=0 when byteNum=32 (power of 2)");
    
    arch_tensor_destroy(tensor);
}

void test_zero_dimension_normalization() {
    printf("\n=== Test: Zero dimension normalization ===\n");
    
    arch_tensor_t* tensor = arch_tensor_create_random();
    tensor->tensorDesc.dimension[1] = 0;
    
    microarch_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = arch_tensor_convert_with_constraints(tensor, NULL, NULL, &result);
    
    print_conversion_result(tensor, &result);
    
    ASSERT(ret == E_SUCCESS, "Conversion succeeds");
    ASSERT(result.desc.unitNum >= 1, "unitNum normalized to >= 1");
    
    arch_tensor_destroy(tensor);
}

void test_constraint_adjustment() {
    printf("\n=== Test: Constraint adjustment ===\n");
    
    arch_tensor_t* tensor = arch_tensor_create_random();
    tensor->tensorDesc.dimension[1] = 40;
    tensor->tensorDesc.dimension[2] = 10;
    
    microarch_constraints_t constraints;
    constraints.maxUnitNum = 16;
    constraints.maxSliceNum = 8;
    constraints.maxPlaneNum = 0;
    constraints.maxCubeNum = 0;
    constraints.maxTotalBytes = 0;
    
    microarch_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = arch_tensor_convert_with_constraints(tensor, &constraints, NULL, &result);
    
    print_conversion_result(tensor, &result);
    printf("ret=%d, unitNum=%d, sliceNum=%d\n", ret, result.desc.unitNum, result.desc.sliceNum);
    
    ASSERT(ret == E_SUCCESS, "Conversion succeeds with constraints");
    
    arch_tensor_destroy(tensor);
}

void test_over_constrained() {
    printf("\n=== Test: Over-constrained scenario ===\n");
    
    arch_tensor_t* tensor = arch_tensor_create_random();
    tensor->tensorDesc.dimension[0] = 64;
    tensor->tensorDesc.dimension[1] = 40;
    tensor->tensorDesc.dimension[2] = 10;
    tensor->tensorDesc.dimension[3] = 28;
    tensor->tensorDesc.dimension[4] = 1;
    
    microarch_constraints_t constraints;
    constraints.maxUnitNum = 16;
    constraints.maxSliceNum = 4;
    constraints.maxPlaneNum = 14;
    constraints.maxCubeNum = 0;
    constraints.maxTotalBytes = 0;
    
    microarch_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = arch_tensor_convert_with_constraints(tensor, &constraints, NULL, &result);
    
    print_conversion_result(tensor, &result);
    printf("Return code: %d, errorCode: %d\n", ret, result.errorCode);
    
    arch_tensor_destroy(tensor);
}

void test_sub_tensor_conversion() {
    printf("\n=== Test: Sub-tensor conversion with constraints ===\n");
    
    arch_tensor_t* tensor = arch_tensor_create_random();
    arch_tensor_gen_sub_tensor(tensor);
    
    microarch_constraints_t constraints;
    constraints.maxUnitNum = 0;
    constraints.maxSliceNum = 0;
    constraints.maxPlaneNum = 0;
    constraints.maxCubeNum = 0;
    constraints.maxTotalBytes = 0;
    
    microarch_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = arch_tensor_convert_sub_with_constraints(tensor, &constraints, NULL, &result);
    
    printf("    arch: range[%u,%u,%u,%u,%u]\n", 
           tensor->subTensorDesc.range[0], tensor->subTensorDesc.range[1],
           tensor->subTensorDesc.range[2], tensor->subTensorDesc.range[3],
           tensor->subTensorDesc.range[4]);
    printf("    micro: byteNum=%d, unitNum=%d, sliceNum=%d, planeNum=%d, cubeNum=%d\n",
           result.desc.byteNum, result.desc.unitNum, result.desc.sliceNum,
           result.desc.planeNum, result.desc.cubeNum);
    if (result.desc.byteNum > 0 && result.desc.sliceNum > 0 && 
        result.desc.planeNum > 0 && result.desc.cubeNum > 0) {
        printf("    micro skip: [unitSkip=%d, sliceSkip=%d, planeSkip=%d, cubeSkip=%d]\n",
               result.desc.unitSkip, result.desc.sliceSkip,
               result.desc.planeSkip, result.desc.cubeSkip);
    }
    printf("    hasGap: %d\n", result.hasGap);
    
    ASSERT(ret == E_SUCCESS, "Sub-tensor conversion succeeds");
    ASSERT(result.errorCode == E_SUCCESS, "Error code is E_SUCCESS");
    
    arch_tensor_destroy(tensor);
}

void test_dimension_expansion() {
    printf("\n=== Test: Dimension expansion (dim[0] > 64) ===\n");
    
    arch_tensor_t* tensor = arch_tensor_create_random();
    tensor->tensorDesc.dimension[0] = 512;
    tensor->tensorDesc.dimension[1] = 512;
    tensor->tensorDesc.dimension[2] = 1;
    tensor->tensorDesc.dimension[3] = 1;
    tensor->tensorDesc.dimension[4] = 1;
    
    unsigned long long arch_bytes = 512ULL * 512ULL;
    
    microarch_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = arch_tensor_convert_with_constraints(tensor, NULL, NULL, &result);
    
    print_conversion_result(tensor, &result);
    printf("    total bytes: %llu -> %llu\n", 
           arch_bytes,
           (unsigned long long)result.desc.byteNum * result.desc.unitNum);
    
    ASSERT(ret == E_SUCCESS, "Conversion succeeds");
    ASSERT(result.desc.byteNum == 64, "byteNum clamped to 64");
    ASSERT(result.desc.unitNum == 1024, "unitNum expanded to max (1024)");
    ASSERT(result.desc.sliceNum == 4, "sliceNum expanded to 4");
    
    unsigned long long micro_bytes = (unsigned long long)result.desc.byteNum * result.desc.unitNum * result.desc.sliceNum;
    ASSERT(micro_bytes == arch_bytes, "Data volume preserved");
    
    arch_tensor_destroy(tensor);
}

void test_physical_limits() {
    printf("\n=== Test: Physical limits with auto-expansion ===\n");
    
    arch_tensor_t* tensor = arch_tensor_create_random();
    tensor->tensorDesc.dimension[0] = 64;
    tensor->tensorDesc.dimension[1] = 10000;
    tensor->tensorDesc.dimension[2] = 1;
    tensor->tensorDesc.dimension[3] = 1;
    tensor->tensorDesc.dimension[4] = 1;
    tensor->tensorDesc.stride[0] = 64;
    tensor->tensorDesc.stride[1] = 640000;
    tensor->tensorDesc.stride[2] = 640000;
    tensor->tensorDesc.stride[3] = 640000;
    tensor->tensorDesc.stride[4] = 640000;
    
    microarch_physical_limits_t limits;
    limits.maxPhysicalByteNum = 64;
    limits.maxPhysicalUnitNum = 4096;
    limits.maxPhysicalSliceNum = 1024;
    limits.maxPhysicalPlaneNum = 1024;
    limits.maxPhysicalCubeNum = 1024;
    
    microarch_conversion_result_t result;
    memset(&result, 0, sizeof(result));
    
    int ret = arch_tensor_convert_with_constraints(tensor, NULL, &limits, &result);
    
    print_conversion_result(tensor, &result);
    
    ASSERT(ret == E_SUCCESS, "Auto-expansion should succeed");
    ASSERT(result.desc.byteNum == 64, "byteNum stays at max");
    ASSERT((unsigned int)result.desc.unitNum <= limits.maxPhysicalUnitNum, "unitNum within limit");
    
    unsigned long long arch_bytes = 64ULL * 10000ULL;
    unsigned long long micro_bytes = (unsigned long long)result.desc.byteNum * 
                                      result.desc.unitNum * 
                                      result.desc.sliceNum * 
                                      result.desc.planeNum * 
                                      result.desc.cubeNum;
    ASSERT(micro_bytes >= arch_bytes, "Micro bytes >= arch bytes (may have extra from rounding)");
    
    arch_tensor_destroy(tensor);
}

int main(void) {
    printf("=== Constraint System Tests ===\n");
    
    test_no_constraints();
    test_byte_num_exceeded();
    test_has_gap_detection();
    test_zero_dimension_normalization();
    test_constraint_adjustment();
    test_over_constrained();
    test_sub_tensor_conversion();
    test_dimension_expansion();
    test_physical_limits();
    
    printf("\n=== Results: %d/%d passed ===\n", pass_count, test_count);
    
    return (pass_count == test_count) ? 0 : 1;
}
