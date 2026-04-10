#include "tensor_descriptor.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* ============================================================================
 * Helper Functions
 * ============================================================================ */
static unsigned int gen_random_data(unsigned int max_value) {
    srand((unsigned int)time(0));
    return (rand() % max_value) + 1;
}

static unsigned int max_val(unsigned int x, unsigned int y) {
    return (x > y) ? x : y;
}

static void compute_arch_strides(const unsigned int* dim, unsigned int* stride) {
    stride[0] = 1 << (int)ceil(log2((double)dim[0]));
    for (int i = 1; i < NDIM; i++) {
        stride[i] = dim[i] * stride[i-1];
    }
}

static int get_skip_addr(int base_addr, int skip) {
    int raw_addr = base_addr + skip;
    int skip_addr = raw_addr & ((1 << 22) - 1);
    return skip_addr;
}

/* ============================================================================
 * Module A: Architecture Tensor Implementation
 * ============================================================================ */
arch_tensor_t* arch_tensor_create_random(void) {
    arch_tensor_t* tensor = (arch_tensor_t*)malloc(sizeof(arch_tensor_t));
    if (!tensor) return NULL;

    tensor->tensorDesc.baseAddr  = gen_random_data(100) * 64;
    tensor->tensorDesc.dimension[0] = gen_random_data(64);
    tensor->tensorDesc.dimension[1] = gen_random_data(40);
    tensor->tensorDesc.dimension[2] = gen_random_data(10);
    tensor->tensorDesc.dimension[3] = gen_random_data(20);
    tensor->tensorDesc.dimension[4] = gen_random_data(6);

    compute_arch_strides(tensor->tensorDesc.dimension, tensor->tensorDesc.stride);

    tensor->fatherMicroarchTensor = NULL;
    tensor->subMicroarchTensor = NULL;

    return tensor;
}

void arch_tensor_destroy(arch_tensor_t* tensor) {
    if (!tensor) return;
    if (tensor->fatherMicroarchTensor) {
        microarch_tensor_destroy(tensor->fatherMicroarchTensor);
    }
    if (tensor->subMicroarchTensor) {
        microarch_tensor_destroy(tensor->subMicroarchTensor);
    }
    free(tensor);
}

void arch_tensor_gen_sub_tensor(arch_tensor_t* tensor) {
    if (!tensor) return;

    tensor->subTensorDesc.coords[0] = gen_random_data(tensor->tensorDesc.dimension[0]);
    tensor->subTensorDesc.coords[1] = gen_random_data(tensor->tensorDesc.dimension[1]);
    tensor->subTensorDesc.coords[2] = gen_random_data(tensor->tensorDesc.dimension[2]);
    tensor->subTensorDesc.coords[3] = gen_random_data(tensor->tensorDesc.dimension[3]);
    tensor->subTensorDesc.coords[4] = gen_random_data(tensor->tensorDesc.dimension[4]);

    tensor->subTensorDesc.range[0]  = gen_random_data(max_val(1, tensor->tensorDesc.dimension[0] - tensor->subTensorDesc.coords[0]));
    tensor->subTensorDesc.range[1]  = gen_random_data(max_val(1, tensor->tensorDesc.dimension[1] - tensor->subTensorDesc.coords[1]));
    tensor->subTensorDesc.range[2]  = gen_random_data(max_val(1, tensor->tensorDesc.dimension[2] - tensor->subTensorDesc.coords[2]));
    tensor->subTensorDesc.range[3]  = gen_random_data(max_val(1, tensor->tensorDesc.dimension[3] - tensor->subTensorDesc.coords[3]));
    tensor->subTensorDesc.range[4]  = gen_random_data(max_val(1, tensor->tensorDesc.dimension[4] - tensor->subTensorDesc.coords[4]));

    tensor->subTensorDesc.traversalStride[0] = 1;
    tensor->subTensorDesc.traversalStride[1] = 1;
    tensor->subTensorDesc.traversalStride[2] = 1;
    tensor->subTensorDesc.traversalStride[3] = 1;
    tensor->subTensorDesc.traversalStride[4] = 1;
}

int arch_tensor_convert_with_constraints(arch_tensor_t* tensor, 
                                          const microarch_constraints_t* constraints,
                                          const microarch_physical_limits_t* physicalLimits,
                                          microarch_conversion_result_t* result) {
    if (!tensor || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    return microarch_constraints_convert(tensor->tensorDesc.dimension,
                                         tensor->tensorDesc.stride,
                                         tensor->tensorDesc.baseAddr,
                                         constraints,
                                         physicalLimits,
                                         result);
}

int arch_tensor_convert_sub_with_constraints(arch_tensor_t* tensor,
                                               const microarch_constraints_t* constraints,
                                               const microarch_physical_limits_t* physicalLimits,
                                               microarch_conversion_result_t* result) {
    if (!tensor || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    unsigned int baseAddr = tensor->subTensorDesc.coords[0] * tensor->tensorDesc.stride[0] +
                            tensor->subTensorDesc.coords[1] * tensor->tensorDesc.stride[1] +
                            tensor->subTensorDesc.coords[2] * tensor->tensorDesc.stride[2] +
                            tensor->subTensorDesc.coords[3] * tensor->tensorDesc.stride[3] +
                            tensor->subTensorDesc.coords[4] * tensor->tensorDesc.stride[4];
    
    int ret = microarch_constraints_convert(tensor->subTensorDesc.range,
                                            tensor->tensorDesc.stride,
                                            baseAddr,
                                            constraints,
                                            physicalLimits,
                                            result);
    
    if (ret == E_SUCCESS) {
        result->desc.unitSkip = (int)(tensor->tensorDesc.stride[0] * tensor->subTensorDesc.traversalStride[0]);
        result->desc.sliceSkip = (int)(tensor->tensorDesc.stride[1] * tensor->subTensorDesc.traversalStride[1]);
        result->desc.planeSkip = (int)(tensor->tensorDesc.stride[2] * tensor->subTensorDesc.traversalStride[2]);
        result->desc.cubeSkip = (int)(tensor->tensorDesc.stride[3] * tensor->subTensorDesc.traversalStride[3]);
    }
    
    return ret;
}

/* ============================================================================
 * Module B: Microarchitecture Tensor Implementation
 * ============================================================================ */
microarch_tensor_t* microarch_tensor_create_random(void) {
    microarch_tensor_t* tensor = (microarch_tensor_t*)malloc(sizeof(microarch_tensor_t));
    if (!tensor) return NULL;

    tensor->tensorDesc.baseAddr  = gen_random_data(100) * 64;
    tensor->tensorDesc.byteNum   = gen_random_data(64);
    tensor->tensorDesc.unitNum   = gen_random_data(40);
    tensor->tensorDesc.sliceNum  = gen_random_data(10);
    tensor->tensorDesc.planeNum  = gen_random_data(20);
    tensor->tensorDesc.cubeNum   = gen_random_data(6);

    tensor->tensorDesc.unitSkip  = 1 << (int)ceil(log2((double)tensor->tensorDesc.byteNum));
    tensor->tensorDesc.sliceSkip = tensor->tensorDesc.unitNum * tensor->tensorDesc.unitSkip;
    tensor->tensorDesc.planeSkip = tensor->tensorDesc.sliceNum * tensor->tensorDesc.sliceSkip;
    tensor->tensorDesc.cubeSkip  = tensor->tensorDesc.planeNum * tensor->tensorDesc.planeSkip;

    return tensor;
}

microarch_tensor_t* microarch_tensor_create_from_desc(const microarch_tensor_descriptor_t* desc) {
    if (!desc) return NULL;

    microarch_tensor_t* tensor = (microarch_tensor_t*)malloc(sizeof(microarch_tensor_t));
    if (!tensor) return NULL;

    tensor->tensorDesc.baseAddr  = desc->baseAddr;
    tensor->tensorDesc.byteNum   = desc->byteNum;
    tensor->tensorDesc.unitNum   = desc->unitNum;
    tensor->tensorDesc.sliceNum  = desc->sliceNum;
    tensor->tensorDesc.planeNum  = desc->planeNum;
    tensor->tensorDesc.cubeNum   = desc->cubeNum;
    tensor->tensorDesc.unitSkip  = desc->unitSkip;
    tensor->tensorDesc.sliceSkip = desc->sliceSkip;
    tensor->tensorDesc.planeSkip = desc->planeSkip;
    tensor->tensorDesc.cubeSkip  = desc->cubeSkip;

    return tensor;
}

void microarch_tensor_destroy(microarch_tensor_t* tensor) {
    if (tensor) {
        free(tensor);
    }
}

void microarch_tensor_gen_sub_tensor(microarch_tensor_t* tensor) {
    if (!tensor) return;

    tensor->subTensorDesc.baseAddr  = tensor->tensorDesc.baseAddr;
    tensor->subTensorDesc.byteNum   = gen_random_data(tensor->tensorDesc.byteNum);
    tensor->subTensorDesc.unitNum   = gen_random_data(tensor->tensorDesc.unitNum);
    tensor->subTensorDesc.sliceNum  = gen_random_data(tensor->tensorDesc.sliceNum);
    tensor->subTensorDesc.planeNum  = gen_random_data(tensor->tensorDesc.planeNum);
    tensor->subTensorDesc.cubeNum   = gen_random_data(tensor->tensorDesc.cubeNum);

    tensor->subTensorDesc.unitSkip  = tensor->tensorDesc.unitSkip;
    tensor->subTensorDesc.sliceSkip = tensor->tensorDesc.sliceSkip;
    tensor->subTensorDesc.planeSkip = tensor->tensorDesc.planeSkip;
    tensor->subTensorDesc.cubeSkip  = tensor->tensorDesc.cubeSkip;

    printf("tensor descriptor:\n");
    microarch_tensor_print("tensorDesc", &tensor->tensorDesc);

    printf("sub tensor descriptor:\n");
    microarch_tensor_print("subTensorDesc", &tensor->subTensorDesc);
}

int microarch_tensor_get_traversal_count(const microarch_tensor_descriptor_t* desc) {
    if (!desc) return 0;
    return desc->unitNum * desc->sliceNum * desc->planeNum * desc->cubeNum;
}

int* microarch_tensor_traversal(const microarch_tensor_descriptor_t* myTensorDesc) {
    if (!myTensorDesc) return NULL;

    int count = microarch_tensor_get_traversal_count(myTensorDesc);
    int* traversal_addr = (int*)malloc(count * sizeof(int));
    if (!traversal_addr) return NULL;

    microarch_tensor_print("myTensorDesc", myTensorDesc);

    int cube_addr  = myTensorDesc->baseAddr;
    int plane_addr = cube_addr;
    int slice_addr = cube_addr;
    int unit_addr  = cube_addr;

    for (int cube_cnt = 0; cube_cnt < myTensorDesc->cubeNum; cube_cnt++) {
        printf("cubeCnt= %d\n", cube_cnt);
        plane_addr = cube_addr;
        cube_addr = get_skip_addr(cube_addr, myTensorDesc->cubeSkip);

        for (int plane_cnt = 0; plane_cnt < myTensorDesc->planeNum; plane_cnt++) {
            printf("planeCnt = %d\n", plane_cnt);
            slice_addr = plane_addr;
            plane_addr = get_skip_addr(plane_addr, myTensorDesc->planeSkip);

            for (int slice_cnt = 0; slice_cnt < myTensorDesc->sliceNum; slice_cnt++) {
                printf("sliceCnt = %d\n", slice_cnt);
                unit_addr  = slice_addr;
                slice_addr = get_skip_addr(slice_addr, myTensorDesc->sliceSkip);

                for (int unit_cnt = 0; unit_cnt < myTensorDesc->unitNum; unit_cnt++) {
                    int idx = cube_cnt * myTensorDesc->planeNum * myTensorDesc->sliceNum * myTensorDesc->unitNum +
                              plane_cnt * myTensorDesc->sliceNum * myTensorDesc->unitNum +
                              slice_cnt * myTensorDesc->unitNum + unit_cnt;
                    printf("unitCnt = %4d, sliceCnt = %d, planeCnt = %d, cubeCnt= %d, idx = %8d, unitAddr = 0x%x\n",
                           unit_cnt, slice_cnt, plane_cnt, cube_cnt, idx, unit_addr);
                    traversal_addr[idx] = unit_addr;
                    unit_addr += myTensorDesc->unitSkip;
                }
                printf("***** slice done ******\n\n");
            }
            printf("----- plane done ------\n\n");
        }
        printf("===== cube done ======\n\n");
    }

    return traversal_addr;
}

void microarch_tensor_print(const char* name, const microarch_tensor_descriptor_t* desc) {
    if (!desc) return;
    printf("baseAddr: %d\n", desc->baseAddr);
    printf("byteNum: %d, unitNum: %d, sliceNum: %d, planeNum: %d, cubeNum: %d\n",
           desc->byteNum, desc->unitNum, desc->sliceNum, desc->planeNum, desc->cubeNum);
    printf("unitSkip : %d\n", desc->unitSkip);
    printf("sliceSkip: %d\n", desc->sliceSkip);
    printf("planeSkip: %d\n", desc->planeSkip);
    printf("cubeSkip:  %d\n", desc->cubeSkip);
    printf("\n");
}

/* ============================================================================
 * Internal Constraint Conversion
 * ============================================================================ */
static void compute_effective_limits_internal(const microarch_physical_limits_t* limits,
                                               const microarch_constraints_t* constraints,
                                               unsigned int* effMaxByte,
                                               unsigned int* effMaxUnit,
                                               unsigned int* effMaxSlice,
                                               unsigned int* effMaxPlane,
                                               unsigned int* effMaxCube) {
    *effMaxByte = MAX_BYTE_NUM;
    *effMaxUnit = DEFAULT_MAX_UNIT;
    *effMaxSlice = DEFAULT_MAX_SLICE;
    *effMaxPlane = DEFAULT_MAX_PLANE;
    *effMaxCube = DEFAULT_MAX_CUBE;
    
    if (limits) {
        *effMaxByte = limits->maxPhysicalByteNum > 0 ? limits->maxPhysicalByteNum : *effMaxByte;
        *effMaxUnit = limits->maxPhysicalUnitNum > 0 ? limits->maxPhysicalUnitNum : *effMaxUnit;
        *effMaxSlice = limits->maxPhysicalSliceNum > 0 ? limits->maxPhysicalSliceNum : *effMaxSlice;
        *effMaxPlane = limits->maxPhysicalPlaneNum > 0 ? limits->maxPhysicalPlaneNum : *effMaxPlane;
        *effMaxCube = limits->maxPhysicalCubeNum > 0 ? limits->maxPhysicalCubeNum : *effMaxCube;
    }
    
    if (constraints) {
        if (constraints->maxByteNum > 0 && constraints->maxByteNum < *effMaxByte) *effMaxByte = constraints->maxByteNum;
        if (constraints->maxUnitNum > 0 && constraints->maxUnitNum < *effMaxUnit) *effMaxUnit = constraints->maxUnitNum;
        if (constraints->maxSliceNum > 0 && constraints->maxSliceNum < *effMaxSlice) *effMaxSlice = constraints->maxSliceNum;
        if (constraints->maxPlaneNum > 0 && constraints->maxPlaneNum < *effMaxPlane) *effMaxPlane = constraints->maxPlaneNum;
        if (constraints->maxCubeNum > 0 && constraints->maxCubeNum < *effMaxCube) *effMaxCube = constraints->maxCubeNum;
    }
    
    if (*effMaxPlane == 0) *effMaxPlane = DEFAULT_MAX_PLANE;
    if (*effMaxCube == 0) *effMaxCube = DEFAULT_MAX_CUBE;
}

static void expand_for_physical_limits(unsigned int* dim,
                                       const microarch_physical_limits_t* limits,
                                       const microarch_constraints_t* constraints) {
    unsigned int maxByte, maxUnit, maxSlice, maxPlane, maxCube;
    compute_effective_limits_internal(limits, constraints, &maxByte, &maxUnit, &maxSlice, &maxPlane, &maxCube);
    
    unsigned int origByte = dim[0];
    if (origByte == 0) origByte = 1;
    
    if (origByte > maxByte && maxByte > 0) {
        int extraBytes = origByte - maxByte;
        int extraUnits = (extraBytes + maxByte - 1) / maxByte;
        dim[1] = dim[1] * (1 + extraUnits);
        dim[0] = maxByte;
        origByte = maxByte;
    }
    
    unsigned int newDim[5] = {dim[0], dim[1], dim[2], dim[3], dim[4]};
    for (int i = 0; i < 5; i++) {
        if (newDim[i] == 0) newDim[i] = 1;
    }
    
    unsigned long long total = (unsigned long long)newDim[0] * newDim[1] * newDim[2] * newDim[3] * newDim[4];
    unsigned long long needed = (total + newDim[0] - 1) / newDim[0];
    unsigned long long current = needed;
    
    newDim[1] = (unsigned int)((current > maxUnit) ? maxUnit : current);
    current = (current + newDim[1] - 1) / newDim[1];
    
    newDim[2] = (unsigned int)((current > maxSlice) ? maxSlice : current);
    current = (current + newDim[2] - 1) / newDim[2];
    
    newDim[3] = (unsigned int)((current > maxPlane) ? maxPlane : current);
    current = (current + newDim[3] - 1) / newDim[3];
    
    newDim[4] = (unsigned int)((current > maxCube) ? maxCube : current);
    
    while ((unsigned long long)newDim[0] * newDim[1] * newDim[2] * newDim[3] * newDim[4] < total) {
        int advanced = 0;
        for (int i = 1; i <= 4; i++) {
            unsigned int limit = (i == 1) ? maxUnit : (i == 2) ? maxSlice : (i == 3) ? maxPlane : maxCube;
            if (newDim[i] < limit) {
                newDim[i]++;
                advanced = 1;
                break;
            }
        }
        if (!advanced) break;
    }
    
    for (int i = 0; i < 5; i++) {
        if (newDim[i] == 0) newDim[i] = 1;
    }
    
    dim[0] = newDim[0];
    dim[1] = newDim[1];
    dim[2] = newDim[2];
    dim[3] = newDim[3];
    dim[4] = newDim[4];
}

int microarch_constraints_convert(const unsigned int* archDim,
                                  const unsigned int* archStride,
                                  unsigned int baseAddr,
                                  const microarch_constraints_t* constraints,
                                  const microarch_physical_limits_t* physicalLimits,
                                  microarch_conversion_result_t* result) {
    if (!archDim || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    unsigned int dim[5];
    for (int i = 0; i < 5; i++) {
        dim[i] = archDim[i];
        if (dim[i] == 0) dim[i] = 1;
    }
    
    expand_for_physical_limits(dim, physicalLimits, constraints);
    
    for (int i = 0; i < 5; i++) {
        if (dim[i] == 0) dim[i] = 1;
    }
    
    if (constraints) {
        unsigned long long finalBytes = (unsigned long long)dim[0] * dim[1] * dim[2] * dim[3] * dim[4];
        if (constraints->maxTotalBytes > 0 && finalBytes > constraints->maxTotalBytes) {
            result->errorCode = E_OVER_CONSTRAINED;
            return E_OVER_CONSTRAINED;
        }
    }
    
    result->desc.baseAddr = (int)baseAddr;
    result->desc.byteNum = (int)dim[0];
    result->desc.unitNum = (int)dim[1];
    result->desc.sliceNum = (int)dim[2];
    result->desc.planeNum = (int)dim[3];
    result->desc.cubeNum = (int)dim[4];
    
    compute_effective_limits_internal(physicalLimits, constraints,
                                     &result->effectiveMaxByte,
                                     &result->effectiveMaxUnit,
                                     &result->effectiveMaxSlice,
                                     &result->effectiveMaxPlane,
                                     &result->effectiveMaxCube);
    
    result->desc.unitSkip = 1 << (int)ceil(log2((double)result->desc.byteNum));
    result->desc.sliceSkip = result->desc.unitNum * result->desc.unitSkip;
    result->desc.planeSkip = result->desc.sliceNum * result->desc.sliceSkip;
    result->desc.cubeSkip = result->desc.planeNum * result->desc.planeSkip;
    
    int actualStride = 1 << (int)ceil(log2((double)dim[0]));
    if (dim[0] == 0) {
        result->hasGap = 0;
    } else if (dim[0] == 1) {
        result->hasGap = 1;
    } else {
        result->hasGap = (actualStride > (int)dim[0]) ? 1 : 0;
    }
    result->errorCode = E_SUCCESS;
    
    return E_SUCCESS;
}

/* ============================================================================
 * Public API Implementation
 * ============================================================================ */
int tensor_descriptor_convert(const tensor_descriptor_t* desc,
                             const tensor_constraints_t* constraints,
                             const tensor_physical_limits_t* limits,
                             tensor_conversion_result_t* result) {
    if (!desc || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    /* Convert tensor_constraints_t to microarch_constraints_t */
    microarch_constraints_t micro_constraints = {0};
    if (constraints) {
        micro_constraints.maxByteNum = constraints->maxByteNum;
        micro_constraints.maxUnitNum = constraints->maxUnitNum;
        micro_constraints.maxSliceNum = constraints->maxSliceNum;
        micro_constraints.maxPlaneNum = constraints->maxPlaneNum;
        micro_constraints.maxCubeNum = constraints->maxCubeNum;
        micro_constraints.maxTotalBytes = constraints->maxTotalBytes;
    }
    
    /* Convert tensor_physical_limits_t to microarch_physical_limits_t */
    microarch_physical_limits_t micro_limits = {0};
    if (limits) {
        micro_limits.maxPhysicalByteNum = limits->maxPhysicalByteNum;
        micro_limits.maxPhysicalUnitNum = limits->maxPhysicalUnitNum;
        micro_limits.maxPhysicalSliceNum = limits->maxPhysicalSliceNum;
        micro_limits.maxPhysicalPlaneNum = limits->maxPhysicalPlaneNum;
        micro_limits.maxPhysicalCubeNum = limits->maxPhysicalCubeNum;
    }
    
    /* Call internal conversion */
    microarch_conversion_result_t micro_result = {0};
    int ret = microarch_constraints_convert(desc->dimension,
                                            desc->stride,
                                            desc->baseAddr,
                                            constraints ? &micro_constraints : NULL,
                                            limits ? &micro_limits : NULL,
                                            &micro_result);
    
    /* Convert result back to tensor_conversion_result_t */
    result->desc.baseAddr = micro_result.desc.baseAddr;
    result->desc.byteNum = micro_result.desc.byteNum;
    result->desc.unitNum = micro_result.desc.unitNum;
    result->desc.sliceNum = micro_result.desc.sliceNum;
    result->desc.planeNum = micro_result.desc.planeNum;
    result->desc.cubeNum = micro_result.desc.cubeNum;
    result->desc.unitSkip = micro_result.desc.unitSkip;
    result->desc.sliceSkip = micro_result.desc.sliceSkip;
    result->desc.planeSkip = micro_result.desc.planeSkip;
    result->desc.cubeSkip = micro_result.desc.cubeSkip;
    result->hasGap = micro_result.hasGap;
    result->errorCode = micro_result.errorCode;
    result->effectiveMaxByte = micro_result.effectiveMaxByte;
    result->effectiveMaxUnit = micro_result.effectiveMaxUnit;
    result->effectiveMaxSlice = micro_result.effectiveMaxSlice;
    result->effectiveMaxPlane = micro_result.effectiveMaxPlane;
    result->effectiveMaxCube = micro_result.effectiveMaxCube;
    
    return ret;
}

int tensor_descriptor_convert_sub(const tensor_descriptor_t* desc,
                                   const tensor_constraints_t* constraints,
                                   const tensor_physical_limits_t* limits,
                                   tensor_conversion_result_t* result) {
    /* TODO: Implement sub-tensor conversion using arch_tensor_convert_sub_with_constraints */
    if (!desc || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    /* For now, just use the main conversion */
    return tensor_descriptor_convert(desc, constraints, limits, result);
}
