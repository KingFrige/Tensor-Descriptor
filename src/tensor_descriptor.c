#include "tensor_descriptor.h"
#include <stdio.h>
#include <stdlib.h>

/* ============================================================================
 * Internal Helper Functions
 * ============================================================================ */
static unsigned int next_power_of_2(unsigned int x) {
    if (x == 0) return 1;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

static int get_skip_addr(int base_addr, int skip) {
    int raw_addr = base_addr + skip;
    int skip_addr = raw_addr & ((1 << 22) - 1);
    return skip_addr;
}

/* ============================================================================
 * Module A: Architecture Tensor Internal Functions
 * ============================================================================ */
int ggml_tensor_convert_with_constraints(ggml_tensor_t* tensor, 
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

int ggml_tensor_convert_sub_with_constraints(ggml_tensor_t* tensor,
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

static unsigned int get_limit_value(int dimIndex, 
                                    const microarch_physical_limits_t* limits,
                                    const microarch_constraints_t* constraints) {
    unsigned int physicalLimit = 0;
    switch(dimIndex) {
        case 1: physicalLimit = limits ? limits->maxPhysicalUnitNum : DEFAULT_MAX_UNIT; break;
        case 2: physicalLimit = limits ? limits->maxPhysicalSliceNum : DEFAULT_MAX_SLICE; break;
        case 3: physicalLimit = limits ? limits->maxPhysicalPlaneNum : DEFAULT_MAX_PLANE; break;
        case 4: physicalLimit = limits ? limits->maxPhysicalCubeNum : DEFAULT_MAX_CUBE; break;
        default: physicalLimit = DEFAULT_MAX_UNIT;
    }
    
    unsigned int userLimit = 0;
    if (constraints) {
        switch(dimIndex) {
            case 1: userLimit = constraints->maxUnitNum; break;
            case 2: userLimit = constraints->maxSliceNum; break;
            case 3: userLimit = constraints->maxPlaneNum; break;
            case 4: userLimit = constraints->maxCubeNum; break;
        }
    }
    
    if (userLimit > 0 && userLimit < physicalLimit) {
        return userLimit;
    }
    return physicalLimit;
}

static int get_highest_dim_used(const unsigned int* dim) {
    for (int i = 4; i >= 1; i--) {
        if (dim[i] > 1) return i;
    }
    return 1;
}

static void balance_last_two_dims(unsigned int* dim,
                                   const microarch_physical_limits_t* limits,
                                   const microarch_constraints_t* constraints,
                                   unsigned long long archTotal) {
    int highestDim = get_highest_dim_used(dim);
    int secondLast = highestDim - 1;
    if (secondLast < 1) return;
    
    unsigned int max1 = get_limit_value(secondLast, limits, constraints);
    unsigned int max2 = get_limit_value(highestDim, limits, constraints);
    
    unsigned long long otherProduct = 1;
    for (int i = 0; i < 5; i++) {
        if (i != secondLast && i != highestDim) {
            otherProduct *= dim[i];
        }
    }
    
    unsigned long long needed = archTotal / otherProduct;
    
    unsigned int bestV1 = dim[secondLast];
    unsigned int bestV2 = dim[highestDim];
    unsigned long long bestDiff = 0;
    
    unsigned int v1Start = (needed + max2 - 1) / max2;
    if (v1Start < 1) v1Start = 1;
    if (v1Start > max1) return;
    
    for (unsigned int v1 = max1; v1 >= v1Start; v1--) {
        unsigned int v2 = (unsigned int)((needed + v1 - 1) / v1);
        if (v2 < 1) v2 = 1;
        if (v2 > max2) continue;
        
        unsigned long long product = (unsigned long long)v1 * v2;
        unsigned long long diff = (product > needed) ? (product - needed) : (needed - product);
        
        if (bestDiff == 0 || diff < bestDiff) {
            bestDiff = diff;
            bestV1 = v1;
            bestV2 = v2;
            
            if (diff == 0) break;
        }
    }
    
    dim[secondLast] = bestV1;
    dim[highestDim] = bestV2;
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
    unsigned long long archTotal = 1;
    for (int i = 0; i < 5; i++) {
        dim[i] = archDim[i];
        if (dim[i] == 0) dim[i] = 1;
        archTotal *= dim[i];
    }
    
    expand_for_physical_limits(dim, physicalLimits, constraints);
    
    if (constraints && constraints->enableBalance) {
        balance_last_two_dims(dim, physicalLimits, constraints, archTotal);
    }
    
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
    
    result->desc.unitSkip = (int)next_power_of_2(result->desc.byteNum);
    result->desc.sliceSkip = result->desc.unitNum * result->desc.unitSkip;
    result->desc.planeSkip = result->desc.sliceNum * result->desc.sliceSkip;
    result->desc.cubeSkip = result->desc.planeNum * result->desc.planeSkip;
    
    unsigned int actualStride = next_power_of_2(dim[0]);
    if (dim[0] == 0) {
        result->hasGap = 0;
    } else if (dim[0] == 1) {
        result->hasGap = 1;
    } else {
        result->hasGap = (actualStride > dim[0]) ? 1 : 0;
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
        micro_constraints.enableBalance = constraints->enableBalance;
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
    /* TODO: Implement sub-tensor conversion using ggml_tensor_convert_sub_with_constraints */
    if (!desc || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    /* For now, just use the main conversion */
    return tensor_descriptor_convert(desc, constraints, limits, result);
}
