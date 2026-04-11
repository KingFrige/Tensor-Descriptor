#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tensor_descriptor.h"
#include "utils.h"

int pass_count = 0;
int test_count = 0;

static void compute_arch_stride(const unsigned int* dim, unsigned int* stride) {
    stride[0] = next_power_of_2(dim[0]);
    for (int i = 1; i < 5; i++) {
        stride[i] = stride[i-1] * dim[i];
    }
}

static void print_conversion_result(const char* name, 
                                    const unsigned int* archDim,
                                    const tensor_conversion_result_t* result) {
    unsigned int stride[5];
    compute_arch_stride(archDim, stride);
    
    printf("  [ %s ]\n", name);
    printf("    arch: dim[%u,%u,%u,%u,%u]\n", 
           archDim[0], archDim[1], archDim[2], archDim[3], archDim[4]);
    printf("    arch stride: [%u,%u,%u,%u,%u]\n",
           stride[0], stride[1], stride[2], stride[3], stride[4]);
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
    } else {
        printf("    (conversion failed)\n");
    }
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

void test_physical_limits_batch() {
    printf("\n=== Batch Test: Physical limits (byteNum=64, others=1024) ===\n");
    
    tensor_physical_limits_t limits;
    limits.maxPhysicalByteNum = 64;
    limits.maxPhysicalUnitNum = 1024;
    limits.maxPhysicalSliceNum = 1024;
    limits.maxPhysicalPlaneNum = 1024;
    limits.maxPhysicalCubeNum = 1024;
    
    struct {
        const char* name;
        unsigned int dim[5];
        int expect_pass;
        int expect_byte;
        int expect_unit;
    } cases[] = {
        {"dim[0]=64 OK", {64, 1, 1, 1, 1}, 1, 64, 1},
        {"dim[0]=64*1024 OK", {64, 1024, 1, 1, 1}, 1, 64, 1024},
        {"dim[0]=64*1025 adjust", {64, 1025, 1, 1, 1}, 1, 64, 1024},
        {"sliceNum=1024 OK", {64, 1, 1024, 1, 1}, 1, 64, 1024},
        {"sliceNum=1025 adjust", {64, 1, 1025, 1, 1}, 1, 64, 1024},
        {"planeNum=1024 OK", {64, 1, 1, 1024, 1}, 1, 64, 1024},
        {"planeNum=1025 adjust", {64, 1, 1, 1025, 1}, 1, 64, 1024},
        {"cubeNum=1024 OK", {64, 1, 1, 1, 1024}, 1, 64, 1024},
        {"cubeNum=1025 adjust", {64, 1, 1, 1, 1025}, 1, 64, 1024},
        {"edge case: 64*512 OK", {64, 512, 1, 1, 1}, 1, 64, 512},
        {"edge case: 64*512*2 OK", {64, 512, 2, 1, 1}, 1, 64, 1024},
        {"edge case: 64*1025 slice adjust", {64, 1, 1025, 1, 1}, 1, 64, 1024},
    };
    
    int num_cases = sizeof(cases) / sizeof(cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        printf("\n--- Case: %s ---\n", cases[i].name);
        
        tensor_descriptor_t desc;
        unsigned int stride[5];
        compute_arch_stride(cases[i].dim, stride);
        
        desc.baseAddr = 5120;
        for (int j = 0; j < 5; j++) {
            desc.dimension[j] = cases[i].dim[j];
            desc.stride[j] = stride[j];
        }
        
        tensor_conversion_result_t result;
        memset(&result, 0, sizeof(result));
        
        int ret = tensor_descriptor_convert(&desc, NULL, &limits, &result);
        
        print_conversion_result(cases[i].name, cases[i].dim, &result);
        
        if (cases[i].expect_pass) {
            ASSERT(ret == E_SUCCESS, "Conversion should succeed (adjusted)");
            if (cases[i].expect_byte > 0) {
                ASSERT(result.desc.byteNum == cases[i].expect_byte, "byteNum correct");
            }
            if (cases[i].expect_unit > 0) {
                ASSERT(result.desc.unitNum == cases[i].expect_unit, "unitNum correct");
            }
        } else {
            ASSERT(ret == E_PHYSICAL_CONSTRAINT, "Should return E_PHYSICAL_CONSTRAINT");
        }
    }
}

void test_dimension_expansion_batch() {
    printf("\n=== Batch Test: Dimension expansion (dim[0] > 64 with physical limits) ===\n");
    
    tensor_physical_limits_t limits;
    limits.maxPhysicalByteNum = 64;
    limits.maxPhysicalUnitNum = 1024;
    limits.maxPhysicalSliceNum = 1024;
    limits.maxPhysicalPlaneNum = 1024;
    limits.maxPhysicalCubeNum = 1024;
    
    struct {
        const char* name;
        unsigned int dim[5];
        int expect_byte;
        int expect_unit;
        int expect_slice;
    } cases[] = {
        {"dim[0]=64", {64, 1, 1, 1, 1}, 64, 1, 1},
        {"dim[0]=128", {128, 1, 1, 1, 1}, 64, 2, 1},
        {"dim[0]=256", {256, 1, 1, 1, 1}, 64, 4, 1},
        {"dim[0]=512", {512, 1, 1, 1, 1}, 64, 8, 1},
        {"dim[0]=1024", {1024, 1, 1, 1, 1}, 64, 16, 1},
        {"dim[0]=64*100", {64, 100, 1, 1, 1}, 64, 100, 1},
        {"dim[0]=128*100", {128, 100, 1, 1, 1}, 64, 200, 1},
        {"dim[0]=512*512", {512, 512, 1, 1, 1}, 64, 1024, 4},
        {"dim[0]=64*1024 OK", {64, 1024, 1, 1, 1}, 64, 1024, 1},
    };
    
    int num_cases = sizeof(cases) / sizeof(cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        printf("\n--- Case: %s ---\n", cases[i].name);
        
        tensor_descriptor_t desc;
        unsigned int stride[5];
        compute_arch_stride(cases[i].dim, stride);
        
        desc.baseAddr = 5120;
        for (int j = 0; j < 5; j++) {
            desc.dimension[j] = cases[i].dim[j];
            desc.stride[j] = stride[j];
        }
        
        unsigned long long arch_total = 1;
        for (int j = 0; j < 5; j++) {
            arch_total *= cases[i].dim[j];
        }
        
        tensor_conversion_result_t result;
        memset(&result, 0, sizeof(result));
        
        int ret = tensor_descriptor_convert(&desc, NULL, &limits, &result);
        
        print_conversion_result(cases[i].name, cases[i].dim, &result);
        
        ASSERT(ret == E_SUCCESS, "Conversion succeeds");
        ASSERT(result.desc.byteNum == cases[i].expect_byte, "byteNum = 64");
        ASSERT(result.desc.unitNum == cases[i].expect_unit, "unitNum within limit");
        if (cases[i].expect_slice > 0) {
            ASSERT(result.desc.sliceNum == cases[i].expect_slice, "sliceNum expanded");
        }
        
        unsigned long long micro_total = (unsigned long long)result.desc.byteNum * 
                                         result.desc.unitNum * result.desc.sliceNum *
                                         result.desc.planeNum * result.desc.cubeNum;
        ASSERT(arch_total == micro_total, "Data volume preserved");
    }
}

void test_byte_sizes_batch() {
    printf("\n=== Batch Test: Various byte sizes ===\n");
    
    struct {
        const char* name;
        int dim0;
    } cases[] = {
        {"byteNum=1", 1},
        {"byteNum=2", 2},
        {"byteNum=3", 3},
        {"byteNum=4", 4},
        {"byteNum=7", 7},
        {"byteNum=8", 8},
        {"byteNum=15", 15},
        {"byteNum=16", 16},
        {"byteNum=31", 31},
        {"byteNum=32", 32},
        {"byteNum=33", 33},
        {"byteNum=63", 63},
        {"byteNum=64", 64},
    };
    
    int num_cases = sizeof(cases) / sizeof(cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        printf("\n--- Case: %s ---\n", cases[i].name);
        
        tensor_descriptor_t desc;
        desc.baseAddr = 5120;
        desc.dimension[0] = cases[i].dim0;
        desc.dimension[1] = 1;
        desc.dimension[2] = 1;
        desc.dimension[3] = 1;
        desc.dimension[4] = 1;
        
        unsigned int stride[5];
        compute_arch_stride(desc.dimension, stride);
        for (int j = 0; j < 5; j++) {
            desc.stride[j] = stride[j];
        }
        
        tensor_conversion_result_t result;
        memset(&result, 0, sizeof(result));
        
        int ret = tensor_descriptor_convert(&desc, NULL, NULL, &result);
        
        unsigned int archDim[5] = {cases[i].dim0, 1, 1, 1, 1};
        print_conversion_result(cases[i].name, archDim, &result);
        
        ASSERT(ret == E_SUCCESS, "Conversion succeeds");
        ASSERT(result.desc.unitSkip == cases[i].dim0, "unitSkip equals byteNum");
    }
}

void test_user_constraints_batch() {
    printf("\n=== Batch Test: User constraints ===\n");
    
    struct {
        const char* name;
        unsigned int dim[5];
        tensor_constraints_t constraints;
        int expect_pass;
        int expect_unit;
    } cases[] = {
        {"unitNum<=100 OK", 
         {64, 100, 1, 1, 1}, 
         {.maxUnitNum = 100, .maxSliceNum = 0, .maxPlaneNum = 0, .maxCubeNum = 0, .maxTotalBytes = 0},
         1, 100},
        {"unitNum<=50 OK (auto-expand)", 
         {64, 100, 1, 1, 1}, 
         {.maxUnitNum = 50, .maxSliceNum = 0, .maxPlaneNum = 0, .maxCubeNum = 0, .maxTotalBytes = 0},
         1, 50},
        {"sliceNum<=10 OK",
         {64, 1, 10, 1, 1},
         {.maxUnitNum = 0, .maxSliceNum = 10, .maxPlaneNum = 0, .maxCubeNum = 0, .maxTotalBytes = 0},
         1, 10},
        {"sliceNum<=5 OK (auto-expand)",
         {64, 1, 10, 1, 1},
         {.maxUnitNum = 0, .maxSliceNum = 5, .maxPlaneNum = 0, .maxCubeNum = 0, .maxTotalBytes = 0},
         1, 10},
        {"totalBytes<=1024 OK",
         {64, 16, 1, 1, 1},
         {.maxUnitNum = 0, .maxSliceNum = 0, .maxPlaneNum = 0, .maxCubeNum = 0, .maxTotalBytes = 1024},
         1, 16},
        {"totalBytes<=512 FAIL",
         {64, 16, 1, 1, 1},
         {.maxUnitNum = 0, .maxSliceNum = 0, .maxPlaneNum = 0, .maxCubeNum = 0, .maxTotalBytes = 512},
         0, 0},
    };
    
    int num_cases = sizeof(cases) / sizeof(cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        printf("\n--- Case: %s ---\n", cases[i].name);
        
        tensor_descriptor_t desc;
        unsigned int stride[5];
        compute_arch_stride(cases[i].dim, stride);
        
        desc.baseAddr = 5120;
        for (int j = 0; j < 5; j++) {
            desc.dimension[j] = cases[i].dim[j];
            desc.stride[j] = stride[j];
        }
        
        tensor_conversion_result_t result;
        memset(&result, 0, sizeof(result));
        
        int ret = tensor_descriptor_convert(&desc, &cases[i].constraints, NULL, &result);
        
        print_conversion_result(cases[i].name, cases[i].dim, &result);
        
        if (cases[i].expect_pass) {
            ASSERT(ret == E_SUCCESS, "Conversion should succeed");
            if (cases[i].expect_unit > 0 && cases[i].expect_unit <= 1024) {
                ASSERT(result.desc.unitNum <= cases[i].expect_unit, "unitNum within constraint");
            }
        } else {
            ASSERT(ret == E_OVER_CONSTRAINED, "Should return constraint error");
        }
    }
}

void test_combined_constraints_batch() {
    printf("\n=== Batch Test: Combined user + physical constraints ===\n");
    
    tensor_physical_limits_t phys_limits = {
        .maxPhysicalByteNum = 64,
        .maxPhysicalUnitNum = 1024,
        .maxPhysicalSliceNum = 1024,
        .maxPhysicalPlaneNum = 1024,
        .maxPhysicalCubeNum = 1024
    };
    
    struct {
        const char* name;
        unsigned int dim[5];
        tensor_constraints_t user_constraints;
        int expect_pass;
    } cases[] = {
        {"within both", {64, 500, 1, 1, 1}, {0}, 1},
        {"exceeds user", {64, 500, 1, 1, 1}, {.maxUnitNum = 100}, 1},
        {"exceeds physical adjust", {64, 2000, 1, 1, 1}, {0}, 1},
        {"within physical edge", {64, 1024, 1, 1, 1}, {0}, 1},
    };
    
    int num_cases = sizeof(cases) / sizeof(cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        printf("\n--- Case: %s ---\n", cases[i].name);
        
        tensor_descriptor_t desc;
        unsigned int stride[5];
        compute_arch_stride(cases[i].dim, stride);
        
        desc.baseAddr = 5120;
        for (int j = 0; j < 5; j++) {
            desc.dimension[j] = cases[i].dim[j];
            desc.stride[j] = stride[j];
        }
        
        tensor_conversion_result_t result;
        memset(&result, 0, sizeof(result));
        
        int ret = tensor_descriptor_convert(&desc, &cases[i].user_constraints, &phys_limits, &result);
        
        print_conversion_result(cases[i].name, cases[i].dim, &result);
        
        if (cases[i].expect_pass) {
            ASSERT(ret == E_SUCCESS, "Should pass");
        } else {
            ASSERT(ret != E_SUCCESS, "Should fail");
        }
    }
}

void test_balance_batch() {
    printf("\n=== Batch Test: Balance mode ===\n");
    
    tensor_physical_limits_t limits = {0};
    limits.maxPhysicalByteNum = 64;
    limits.maxPhysicalUnitNum = 1024;
    limits.maxPhysicalSliceNum = 1024;
    limits.maxPhysicalPlaneNum = 1024;
    limits.maxPhysicalCubeNum = 1024;
    
    struct {
        const char* name;
        unsigned int dim[5];
        tensor_constraints_t constraints;
        unsigned int expect_unit;
        unsigned int expect_slice;
        double expect_ratio;
    } cases[] = {
        {"balance: dim[64,2000] no balance", 
         {64, 2000, 1, 1, 1}, 
         {.maxUnitNum = 0, .enableBalance = 0},
         1024, 2, 1.024},
        {"balance: dim[64,2000] with balance", 
         {64, 2000, 1, 1, 1}, 
         {.maxUnitNum = 0, .enableBalance = 1},
         1000, 2, 1.0},
        {"balance: dim[64,3000] with balance", 
         {64, 3000, 1, 1, 1}, 
         {.maxUnitNum = 0, .enableBalance = 1},
         1000, 3, 1.0},
    };
    
    int num_cases = sizeof(cases) / sizeof(cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        printf("\n--- Case: %s ---\n", cases[i].name);
        
        tensor_descriptor_t desc;
        unsigned int stride[5];
        compute_arch_stride(cases[i].dim, stride);
        
        desc.baseAddr = 5120;
        for (int j = 0; j < 5; j++) {
            desc.dimension[j] = cases[i].dim[j];
            desc.stride[j] = stride[j];
        }
        
        tensor_conversion_result_t result;
        memset(&result, 0, sizeof(result));
        
        int ret = tensor_descriptor_convert(&desc, &cases[i].constraints, &limits, &result);
        
        printf("  [ %s ] balance=%s\n", cases[i].name, cases[i].constraints.enableBalance ? "ON" : "OFF");
        print_conversion_result(cases[i].name, cases[i].dim, &result);
        
        unsigned long long arch_total = 1;
        for (int j = 0; j < 5; j++) {
            arch_total *= cases[i].dim[j];
        }
        unsigned long long micro_total = (unsigned long long)result.desc.byteNum * 
                                         result.desc.unitNum * result.desc.sliceNum *
                                         result.desc.planeNum * result.desc.cubeNum;
        double ratio = (double)micro_total / arch_total;
        printf("    ratio: %.3f (expected %.3f)\n", ratio, cases[i].expect_ratio);
        
        ASSERT(ret == E_SUCCESS, "Conversion succeeds");
        
        if (cases[i].expect_ratio > 0) {
            int ratio_ok = (ratio <= cases[i].expect_ratio + 0.001 && ratio >= cases[i].expect_ratio - 0.001);
            ASSERT(ratio_ok, "ratio expected");
        }
    }
}

int main(void) {
    printf("=== Batch Constraint Tests ===\n");
    
    test_physical_limits_batch();
    test_dimension_expansion_batch();
    test_byte_sizes_batch();
    test_user_constraints_batch();
    test_combined_constraints_batch();
    test_balance_batch();
    
    printf("\n=== Results: %d/%d passed ===\n", pass_count, test_count);
    
    return (pass_count == test_count) ? 0 : 1;
}
