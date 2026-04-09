#include "arch_tensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

static unsigned int gen_random_data(unsigned int max_value) {
    srand((unsigned int)time(0));
    return (rand() % max_value) + 1;
}

static unsigned int max_val(unsigned int x, unsigned int y) {
    return (x > y) ? x : y;
}

static void compute_arch_strides(const unsigned int* dim, unsigned int* stride);

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

void arch_tensor_convert_to_microarch(arch_tensor_t* tensor) {
    if (!tensor) return;

    microarch_tensor_descriptor_t desc;
    desc.baseAddr  = (int)tensor->tensorDesc.baseAddr;
    desc.byteNum   = (int)tensor->tensorDesc.dimension[0];
    desc.unitNum   = (int)tensor->tensorDesc.dimension[1];
    desc.sliceNum  = (int)tensor->tensorDesc.dimension[2];
    desc.planeNum  = (int)tensor->tensorDesc.dimension[3];
    desc.cubeNum   = (int)tensor->tensorDesc.dimension[4];
    desc.unitSkip  = (int)tensor->tensorDesc.stride[0];
    desc.sliceSkip = (int)tensor->tensorDesc.stride[1];
    desc.planeSkip = (int)tensor->tensorDesc.stride[2];
    desc.cubeSkip  = (int)tensor->tensorDesc.stride[3];

    tensor->fatherMicroarchTensor = microarch_tensor_create_from_desc(&desc);
}

void arch_tensor_convert_sub_to_microarch(arch_tensor_t* tensor) {
    if (!tensor) return;

    microarch_tensor_descriptor_t desc;
    desc.baseAddr  = (int)(tensor->subTensorDesc.coords[0] * tensor->tensorDesc.stride[0] +
                         tensor->subTensorDesc.coords[1] * tensor->tensorDesc.stride[1] +
                         tensor->subTensorDesc.coords[2] * tensor->tensorDesc.stride[2] +
                         tensor->subTensorDesc.coords[3] * tensor->tensorDesc.stride[3] +
                         tensor->subTensorDesc.coords[4] * tensor->tensorDesc.stride[4]);
    desc.byteNum   = (int)tensor->subTensorDesc.range[0];
    desc.unitNum   = (int)tensor->subTensorDesc.range[1];
    desc.sliceNum  = (int)tensor->subTensorDesc.range[2];
    desc.planeNum  = (int)tensor->subTensorDesc.range[3];
    desc.cubeNum   = (int)tensor->subTensorDesc.range[4];
    desc.unitSkip  = (int)(tensor->tensorDesc.stride[0] * tensor->subTensorDesc.traversalStride[0]);
    desc.sliceSkip = (int)(tensor->tensorDesc.stride[1] * tensor->subTensorDesc.traversalStride[1]);
    desc.planeSkip = (int)(tensor->tensorDesc.stride[2] * tensor->subTensorDesc.traversalStride[2]);
    desc.cubeSkip  = (int)(tensor->tensorDesc.stride[3] * tensor->subTensorDesc.traversalStride[3]);

    tensor->subMicroarchTensor = microarch_tensor_create_from_desc(&desc);
}

void arch_tensor_print(const char* name, const arch_tensor_descriptor_t* desc) {
    if (!desc) return;
    printf("baseAddr: %u\n", desc->baseAddr);
    printf("dimension[0]:%u, dimension[1]:%u, dimension[2]:%u, dimension[3]:%u, dimension[4]:%u\n",
           desc->dimension[0], desc->dimension[1], desc->dimension[2], desc->dimension[3], desc->dimension[4]);
    printf("stride[0]:%u, stride[1]:%u, stride[2]:%u, stride[3]:%u, stride[4]:%u\n",
           desc->stride[0], desc->stride[1], desc->stride[2], desc->stride[3], desc->stride[4]);
    printf("\n");
}

static void normalize_arch_dimensions(unsigned int* dim) {
    for (int i = 0; i < NDIM; i++) {
        if (dim[i] == 0) dim[i] = 1;
    }
}

static void compute_arch_strides(const unsigned int* dim, unsigned int* stride) {
    stride[0] = 1 << (int)ceil(log2((double)dim[0]));
    for (int i = 1; i < NDIM; i++) {
        stride[i] = dim[i] * stride[i-1];
    }
}

static int adjust_arch_for_constraints(unsigned int* dim, 
                                        const microarch_constraints_t* constraints,
                                        unsigned long long totalBytes) {
    if (!constraints) return 0;
    
    if (constraints->maxUnitNum > 0 && dim[1] > constraints->maxUnitNum) {
        unsigned int overflow = dim[1] - constraints->maxUnitNum;
        dim[1] = constraints->maxUnitNum;
        if (dim[2] > 0) dim[2] += overflow;
    }
    if (constraints->maxSliceNum > 0 && dim[2] > constraints->maxSliceNum) {
        unsigned int overflow = dim[2] - constraints->maxSliceNum;
        dim[2] = constraints->maxSliceNum;
        if (dim[3] > 0) dim[3] += overflow;
    }
    if (constraints->maxPlaneNum > 0 && dim[3] > constraints->maxPlaneNum) {
        unsigned int overflow = dim[3] - constraints->maxPlaneNum;
        dim[3] = constraints->maxPlaneNum;
        if (dim[4] > 0) dim[4] += overflow;
    }
    if (constraints->maxCubeNum > 0 && dim[4] > constraints->maxCubeNum) {
        dim[4] = constraints->maxCubeNum;
    }
    
    normalize_arch_dimensions(dim);
    return 1;
}

static int validate_arch_constraints(const unsigned int* dim, 
                                     const microarch_constraints_t* constraints) {
    if (!constraints) return E_SUCCESS;
    
    unsigned long long currentBytes = (unsigned long long)dim[0] * dim[1] * 
                                      dim[2] * dim[3] * dim[4];
    
    if (constraints->maxTotalBytes > 0 && currentBytes > constraints->maxTotalBytes) {
        return E_OVER_CONSTRAINED;
    }
    
    if (constraints->maxUnitNum > 0 && dim[1] > constraints->maxUnitNum) {
        return E_OVER_CONSTRAINED;
    }
    if (constraints->maxSliceNum > 0 && dim[2] > constraints->maxSliceNum) {
        return E_OVER_CONSTRAINED;
    }
    if (constraints->maxPlaneNum > 0 && dim[3] > constraints->maxPlaneNum) {
        return E_OVER_CONSTRAINED;
    }
    if (constraints->maxCubeNum > 0 && dim[4] > constraints->maxCubeNum) {
        return E_OVER_CONSTRAINED;
    }
    
    return E_SUCCESS;
}

static int compute_arch_has_gap(const unsigned int* dim) {
    if (dim[0] == 0) return 0;
    if (dim[0] == 1) return 1;
    
    int actualStride = 1 << (int)ceil(log2((double)dim[0]));
    if (actualStride > (int)dim[0]) {
        return 1;
    }
    return 0;
}

static int check_physical_limits(const unsigned int* dim, 
                                  const microarch_physical_limits_t* limits) {
    if (!limits) return E_SUCCESS;
    
    if (limits->maxPhysicalByteNum > 0 && dim[0] > limits->maxPhysicalByteNum) {
        return E_PHYSICAL_CONSTRAINT;
    }
    if (limits->maxPhysicalUnitNum > 0 && dim[1] > limits->maxPhysicalUnitNum) {
        return E_PHYSICAL_CONSTRAINT;
    }
    if (limits->maxPhysicalSliceNum > 0 && dim[2] > limits->maxPhysicalSliceNum) {
        return E_PHYSICAL_CONSTRAINT;
    }
    if (limits->maxPhysicalPlaneNum > 0 && dim[3] > limits->maxPhysicalPlaneNum) {
        return E_PHYSICAL_CONSTRAINT;
    }
    if (limits->maxPhysicalCubeNum > 0 && dim[4] > limits->maxPhysicalCubeNum) {
        return E_PHYSICAL_CONSTRAINT;
    }
    
    return E_SUCCESS;
}

static void merge_limits(const microarch_physical_limits_t* limits,
                        const microarch_constraints_t* constraints,
                        unsigned int* maxByte,
                        unsigned int* maxUnit,
                        unsigned int* maxSlice,
                        unsigned int* maxPlane,
                        unsigned int* maxCube) {
    *maxByte = MAX_BYTE_NUM;
    *maxUnit = DEFAULT_MAX_UNIT;
    *maxSlice = DEFAULT_MAX_SLICE;
    *maxPlane = DEFAULT_MAX_PLANE;
    *maxCube = DEFAULT_MAX_CUBE;
    
    if (limits) {
        *maxByte = limits->maxPhysicalByteNum > 0 ? limits->maxPhysicalByteNum : *maxByte;
        *maxUnit = limits->maxPhysicalUnitNum > 0 ? limits->maxPhysicalUnitNum : *maxUnit;
        *maxSlice = limits->maxPhysicalSliceNum > 0 ? limits->maxPhysicalSliceNum : *maxSlice;
        *maxPlane = limits->maxPhysicalPlaneNum > 0 ? limits->maxPhysicalPlaneNum : *maxPlane;
        *maxCube = limits->maxPhysicalCubeNum > 0 ? limits->maxPhysicalCubeNum : *maxCube;
    }
    
    if (constraints) {
        if (constraints->maxUnitNum > 0 && constraints->maxUnitNum < *maxUnit) *maxUnit = constraints->maxUnitNum;
        if (constraints->maxSliceNum > 0 && constraints->maxSliceNum < *maxSlice) *maxSlice = constraints->maxSliceNum;
        if (constraints->maxPlaneNum > 0 && constraints->maxPlaneNum < *maxPlane) *maxPlane = constraints->maxPlaneNum;
        if (constraints->maxCubeNum > 0 && constraints->maxCubeNum < *maxCube) *maxCube = constraints->maxCubeNum;
    }
    
    if (*maxPlane == 0) *maxPlane = DEFAULT_MAX_PLANE;
    if (*maxCube == 0) *maxCube = DEFAULT_MAX_CUBE;
}

static void compute_effective_limits(const microarch_physical_limits_t* limits,
                                     const microarch_constraints_t* constraints,
                                     unsigned int* effMaxByte,
                                     unsigned int* effMaxUnit,
                                     unsigned int* effMaxSlice,
                                     unsigned int* effMaxPlane,
                                     unsigned int* effMaxCube) {
    merge_limits(limits, constraints, effMaxByte, effMaxUnit, effMaxSlice, effMaxPlane, effMaxCube);
}

static void expand_arch_dimensions(unsigned int* dim, 
                                    const microarch_physical_limits_t* limits,
                                    const microarch_constraints_t* constraints) {
    unsigned int maxByte, maxUnit, maxSlice, maxPlane, maxCube;
    merge_limits(limits, constraints, &maxByte, &maxUnit, &maxSlice, &maxPlane, &maxCube);
    
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

int arch_tensor_convert_with_constraints(arch_tensor_t* tensor, 
                                         const microarch_constraints_t* constraints,
                                         const microarch_physical_limits_t* physicalLimits,
                                         microarch_conversion_result_t* result) {
    if (!tensor || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    unsigned long long totalBytes = (unsigned long long)tensor->tensorDesc.dimension[0] *
                                     tensor->tensorDesc.dimension[1] *
                                     tensor->tensorDesc.dimension[2] *
                                     tensor->tensorDesc.dimension[3] *
                                     tensor->tensorDesc.dimension[4];
    
    unsigned int dim[NDIM];
    for (int i = 0; i < NDIM; i++) {
        dim[i] = tensor->tensorDesc.dimension[i];
    }
    
    expand_arch_dimensions(dim, physicalLimits, constraints);
    normalize_arch_dimensions(dim);
    
    int physResult = check_physical_limits(dim, physicalLimits);
    if (physResult != E_SUCCESS) {
        result->errorCode = physResult;
        return physResult;
    }
    
    if (constraints) {
        unsigned long long newTotalBytes = (unsigned long long)dim[0] * dim[1] * dim[2] * dim[3] * dim[4];
        
        int feasResult = validate_arch_constraints(dim, constraints);
        if (feasResult != E_SUCCESS) {
            result->errorCode = feasResult;
            return feasResult;
        }
    }
    
    result->desc.baseAddr = (int)tensor->tensorDesc.baseAddr;
    result->desc.byteNum = (int)dim[0];
    result->desc.unitNum = (int)dim[1];
    result->desc.sliceNum = (int)dim[2];
    result->desc.planeNum = (int)dim[3];
    result->desc.cubeNum = (int)dim[4];
    
    compute_effective_limits(physicalLimits, constraints,
                           &result->effectiveMaxByte,
                           &result->effectiveMaxUnit,
                           &result->effectiveMaxSlice,
                           &result->effectiveMaxPlane,
                           &result->effectiveMaxCube);
    
    result->desc.unitSkip = 1 << (int)ceil(log2((double)result->desc.byteNum));
    result->desc.sliceSkip = result->desc.unitNum * result->desc.unitSkip;
    result->desc.planeSkip = result->desc.sliceNum * result->desc.sliceSkip;
    result->desc.cubeSkip = result->desc.planeNum * result->desc.planeSkip;
    
    result->hasGap = compute_arch_has_gap(dim);
    result->errorCode = E_SUCCESS;
    
    return E_SUCCESS;
}

int arch_tensor_convert_sub_with_constraints(arch_tensor_t* tensor,
                                              const microarch_constraints_t* constraints,
                                              const microarch_physical_limits_t* physicalLimits,
                                              microarch_conversion_result_t* result) {
    if (!tensor || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    unsigned long long totalBytes = (unsigned long long)tensor->subTensorDesc.range[0] *
                                     tensor->subTensorDesc.range[1] *
                                     tensor->subTensorDesc.range[2] *
                                     tensor->subTensorDesc.range[3] *
                                     tensor->subTensorDesc.range[4];
    
    unsigned int baseAddr = tensor->subTensorDesc.coords[0] * tensor->tensorDesc.stride[0] +
                            tensor->subTensorDesc.coords[1] * tensor->tensorDesc.stride[1] +
                            tensor->subTensorDesc.coords[2] * tensor->tensorDesc.stride[2] +
                            tensor->subTensorDesc.coords[3] * tensor->tensorDesc.stride[3] +
                            tensor->subTensorDesc.coords[4] * tensor->tensorDesc.stride[4];
    
    unsigned int range[NDIM];
    for (int i = 0; i < NDIM; i++) {
        range[i] = tensor->subTensorDesc.range[i];
    }
    
    expand_arch_dimensions(range, physicalLimits, NULL);
    normalize_arch_dimensions(range);
    
    int physResult = check_physical_limits(range, physicalLimits);
    if (physResult != E_SUCCESS) {
        result->errorCode = physResult;
        return physResult;
    }
    
    if (constraints) {
        unsigned long long newTotalBytes = (unsigned long long)range[0] * range[1] * range[2] * range[3] * range[4];
        
        int feasResult = validate_arch_constraints(range, constraints);
        if (feasResult != E_SUCCESS) {
            result->errorCode = feasResult;
            return feasResult;
        }
        
        adjust_arch_for_constraints(range, constraints, newTotalBytes);
    }
    
    result->desc.baseAddr = (int)baseAddr;
    result->desc.byteNum = (int)range[0];
    result->desc.unitNum = (int)range[1];
    result->desc.sliceNum = (int)range[2];
    result->desc.planeNum = (int)range[3];
    result->desc.cubeNum = (int)range[4];
    
    compute_effective_limits(physicalLimits, constraints,
                           &result->effectiveMaxByte,
                           &result->effectiveMaxUnit,
                           &result->effectiveMaxSlice,
                           &result->effectiveMaxPlane,
                           &result->effectiveMaxCube);
    
    result->desc.unitSkip = (int)(tensor->tensorDesc.stride[0] * tensor->subTensorDesc.traversalStride[0]);
    result->desc.sliceSkip = (int)(tensor->tensorDesc.stride[1] * tensor->subTensorDesc.traversalStride[1]);
    result->desc.planeSkip = (int)(tensor->tensorDesc.stride[2] * tensor->subTensorDesc.traversalStride[2]);
    result->desc.cubeSkip = (int)(tensor->tensorDesc.stride[3] * tensor->subTensorDesc.traversalStride[3]);
    
    result->hasGap = compute_arch_has_gap(range);
    result->errorCode = E_SUCCESS;
    
    return E_SUCCESS;
}