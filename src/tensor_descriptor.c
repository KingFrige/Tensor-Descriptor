#include "tensor_descriptor.h"
#include <stdio.h>
#include <stdlib.h>

/* ============================================================================
 * Internal Helper Functions
 * ============================================================================ */

/* 
 * next_power_of_2 - Calculate the next power of 2
 * 
 * NOTE: This function is kept for historical reasons but is no longer used
 * in the main code path. The system now uses direct mapping without 
 * power-of-2 alignment.
 */
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
 * Lookup Tables for Dimension Limits
 * ============================================================================ */
static const size_t constraints_offset_table[] = {
    0,  /* placeholder for index 0 */
    offsetof(microarch_constraints_t, maxUnitNum),
    offsetof(microarch_constraints_t, maxSliceNum),
    offsetof(microarch_constraints_t, maxPlaneNum),
    offsetof(microarch_constraints_t, maxCubeNum)
};

static const size_t physical_limits_offset_table[] = {
    0,  /* placeholder for index 0 */
    offsetof(microarch_physical_limits_t, maxPhysicalUnitNum),
    offsetof(microarch_physical_limits_t, maxPhysicalSliceNum),
    offsetof(microarch_physical_limits_t, maxPhysicalPlaneNum),
    offsetof(microarch_physical_limits_t, maxPhysicalCubeNum)
};

static const unsigned int default_limits_table[] = {
    0,                 /* placeholder for index 0 */
    DEFAULT_MAX_UNIT,
    DEFAULT_MAX_SLICE,
    DEFAULT_MAX_PLANE,
    DEFAULT_MAX_CUBE
};

/* ============================================================================
 * Constraint Conversion Helper
 * ============================================================================ */
static void convert_constraints(const tensor_constraints_t* src,
                                microarch_constraints_t* dst) {
    if (!src || !dst) return;
    
    dst->maxByteNum = src->maxByteNum;
    dst->maxUnitNum = src->maxUnitNum;
    dst->maxSliceNum = src->maxSliceNum;
    dst->maxPlaneNum = src->maxPlaneNum;
    dst->maxCubeNum = src->maxCubeNum;
    dst->maxTotalBytes = src->maxTotalBytes;
    dst->enableBalance = src->enableBalance;
}

static void convert_physical_limits(const tensor_physical_limits_t* src,
                                    microarch_physical_limits_t* dst) {
    if (!src || !dst) return;
    
    dst->maxPhysicalByteNum = src->maxPhysicalByteNum;
    dst->maxPhysicalUnitNum = src->maxPhysicalUnitNum;
    dst->maxPhysicalSliceNum = src->maxPhysicalSliceNum;
    dst->maxPhysicalPlaneNum = src->maxPhysicalPlaneNum;
    dst->maxPhysicalCubeNum = src->maxPhysicalCubeNum;
}

/* ============================================================================
 * Result Copy Helper
 * ============================================================================ */
static void copy_conversion_result(const microarch_conversion_result_t* src,
                                   tensor_conversion_result_t* dst) {
    if (!src || !dst) return;
    
    /* Copy descriptor fields individually */
    dst->desc.baseAddr = src->desc.baseAddr;
    dst->desc.byteNum = src->desc.byteNum;
    dst->desc.unitNum = src->desc.unitNum;
    dst->desc.sliceNum = src->desc.sliceNum;
    dst->desc.planeNum = src->desc.planeNum;
    dst->desc.cubeNum = src->desc.cubeNum;
    dst->desc.unitSkip = src->desc.unitSkip;
    dst->desc.sliceSkip = src->desc.sliceSkip;
    dst->desc.planeSkip = src->desc.planeSkip;
    dst->desc.cubeSkip = src->desc.cubeSkip;
    
    /* Copy other fields */
    dst->errorCode = src->errorCode;
    dst->effectiveMaxByte = src->effectiveMaxByte;
    dst->effectiveMaxUnit = src->effectiveMaxUnit;
    dst->effectiveMaxSlice = src->effectiveMaxSlice;
    dst->effectiveMaxPlane = src->effectiveMaxPlane;
    dst->effectiveMaxCube = src->effectiveMaxCube;
}

/* ============================================================================
 * GGML Direct Mapping (Non Power-of-2 Mode)
 * ============================================================================ */

/**
 * Direct mapping from GGML tensor to microarch tensor
 * 
 * Maps GGML format (ne/nb) directly to microarch format without
 * power-of-2 alignment. Uses nb values directly as skip values.
 * 
 * Mapping rules:
 * - byteNum = type_size (from GGML type)
 * - unitNum = ne[0] / block_size (blocks in dim0)
 * - unitSkip = nb[0] (direct from GGML)
 * - sliceSkip = nb[1] (direct from GGML)
 * - planeSkip = nb[2] (direct from GGML)
 * - cubeSkip = nb[3] (direct from GGML)
 * 
 * Parameters:
 *   ne[4]         - GGML dimensions
 *   nb[4]         - GGML strides in bytes
 *   type_size     - GGML type size in bytes per block
 *   block_size    - GGML block size in elements
 *   baseAddr      - Base address
 *   result        - Output conversion result
 * 
 * Returns:
 *   E_SUCCESS on success, negative error code on failure
 */
int ggml_to_microarch_direct_map(const int64_t* ne,
                                  const size_t* nb,
                                  int type_size,
                                  int block_size,
                                  unsigned int baseAddr,
                                  const ggml_block_combine_config_t* combine_config,
                                  microarch_conversion_result_t* result) {
    if (!ne || !nb || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    /* Calculate total blocks in dimension 0 */
    int total_blocks = (int)((ne[0] + block_size - 1) / block_size);
    
    /* Determine target_byteNum */
    int target_byteNum;
    int max_bn = MAX_BYTE_NUM;  /* Default 64 */
    
    if (combine_config && combine_config->max_byteNum > 0) {
        max_bn = combine_config->max_byteNum;
    }
    
    if (combine_config && combine_config->target_byteNum > 0) {
        /* User-specified target */
        target_byteNum = combine_config->target_byteNum;
        /* Ensure within bounds and aligned to type_size */
        if (target_byteNum > max_bn) target_byteNum = max_bn;
        target_byteNum = (target_byteNum / type_size) * type_size;
    } else {
        /* Auto-select: largest multiple of type_size <= max_bn */
        target_byteNum = (max_bn / type_size) * type_size;
    }
    
    /* Ensure at least one block */
    if (target_byteNum < type_size) {
        target_byteNum = type_size;
    }
    
    /* Calculate block combine parameters */
    int blocks_per_unit = target_byteNum / type_size;
    int unitNum = (total_blocks + blocks_per_unit - 1) / blocks_per_unit;  /* ceil division */
    
    /* 5D mapping from GGML to microarch with block combine */
    result->desc.baseAddr = (int)baseAddr;
    result->desc.byteNum = target_byteNum;      /* Combined byteNum */
    result->desc.unitNum = unitNum;             /* Combined units */
    result->desc.sliceNum = (int)ne[1];
    result->desc.planeNum = (int)ne[2];
    result->desc.cubeNum = (int)ne[3];
    
    /* Skip mapping - combined blocks */
    result->desc.unitSkip = blocks_per_unit * (int)nb[0];
    result->desc.sliceSkip = result->desc.unitNum * result->desc.unitSkip;
    result->desc.planeSkip = result->desc.sliceNum * result->desc.sliceSkip;
    result->desc.cubeSkip = result->desc.planeNum * result->desc.planeSkip;
    
    result->errorCode = E_SUCCESS;
    
    return E_SUCCESS;
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
    if (dimIndex < 1 || dimIndex > 4) return DEFAULT_MAX_UNIT;
    
    /* Get physical limit using lookup table */
    unsigned int physicalLimit;
    if (limits) {
        physicalLimit = *(const unsigned int*)((const char*)limits + 
                          physical_limits_offset_table[dimIndex]);
    } else {
        physicalLimit = default_limits_table[dimIndex];
    }
    
    /* Get user constraint using lookup table */
    unsigned int userLimit = 0;
    if (constraints) {
        userLimit = *(const unsigned int*)((const char*)constraints + 
                     constraints_offset_table[dimIndex]);
    }
    
    return (userLimit > 0 && userLimit < physicalLimit) ? userLimit : physicalLimit;
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

/**
 * Fold and distribute dimensions across 5D microarchitecture hierarchy
 *
 * This function performs three operations:
 * 1. FOLD: Redistributes overflow from constrained dimensions to higher levels
 *    (e.g., byteNum > maxByte gets folded into unitNum)
 *
 * 2. DISTRIBUTE: Uses greedy algorithm to spread total elements across
 *    byte/unit/slice/plane/cube hierarchy respecting physical limits
 *
 * 3. BALANCE (optional): When enableBalance=true, optimizes last two
 *    dimensions for better memory access patterns
 */
int microarch_dim_fold_and_distribute(const unsigned int* archDim,
                                       const unsigned int* archStride,
                                       unsigned int baseAddr,
                                       const microarch_constraints_t* constraints,
                                       const microarch_physical_limits_t* physicalLimits,
                                       microarch_conversion_result_t* result) {
    if (!archDim || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    /* Phase 0: Calculate total elements from input dimensions */
    unsigned int dim[5];
    unsigned long long archTotal = 1;
    for (int i = 0; i < 5; i++) {
        dim[i] = archDim[i];
        if (dim[i] == 0) dim[i] = 1;
        archTotal *= dim[i];
    }
    
    /* Phase 1: FOLD - Handle overflow by folding into higher dimensions
     * Example: byteNum=80 > maxByte=64 → byteNum=64, unitNum*=2
     */
    expand_for_physical_limits(dim, physicalLimits, constraints);
    
    /* Phase 2: DISTRIBUTE - Greedy algorithm to spread across hierarchy
     * Fill each level up to its limit, overflow to next level
     */
    
    /* Phase 3: BALANCE - Optimize last two dimensions if enabled */
    if (constraints && constraints->enableBalance) {
        balance_last_two_dims(dim, physicalLimits, constraints, archTotal);
    }
    
    /* Ensure no zero dimensions */
    for (int i = 0; i < 5; i++) {
        if (dim[i] == 0) dim[i] = 1;
    }
    
    /* Validate total size against maxTotalBytes constraint */
    if (constraints) {
        unsigned long long finalBytes = (unsigned long long)dim[0] * dim[1] * dim[2] * dim[3] * dim[4];
        if (constraints->maxTotalBytes > 0 && finalBytes > constraints->maxTotalBytes) {
            result->errorCode = E_OVER_CONSTRAINED;
            return E_OVER_CONSTRAINED;
        }
    }
    
    /* Populate result descriptor */
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
    
    /* Calculate skips using direct mapping (no power-of-2 alignment)
     * Hardware DMA now supports non-power-of-2 skip values
     */
    result->desc.unitSkip = result->desc.byteNum;
    
    /* Calculate upper level skips */
    result->desc.sliceSkip = result->desc.unitNum * result->desc.unitSkip;
    result->desc.planeSkip = result->desc.sliceNum * result->desc.sliceSkip;
    result->desc.cubeSkip = result->desc.planeNum * result->desc.planeSkip;
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
    
    microarch_constraints_t micro_constraints = {0};
    convert_constraints(constraints, &micro_constraints);
    
    microarch_physical_limits_t micro_limits = {0};
    convert_physical_limits(limits, &micro_limits);
    
    microarch_conversion_result_t micro_result = {0};
    
    int ret = microarch_dim_fold_and_distribute(desc->dimension,
                                                 desc->stride,
                                                 desc->baseAddr,
                                                 constraints ? &micro_constraints : NULL,
                                                 limits ? &micro_limits : NULL,
                                                 &micro_result);
    
    copy_conversion_result(&micro_result, result);
    
    return ret;
}
