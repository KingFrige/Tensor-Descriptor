#include "microarch_tensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

static unsigned int gen_random_data(unsigned int max_value) {
    srand((unsigned int)time(0));
    return (rand() % max_value) + 1;
}

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

static int get_skip_addr(int base_addr, int skip) {
    int raw_addr = base_addr + skip;
    int skip_addr = raw_addr & ((1 << 22) - 1);
    return skip_addr;
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

static int check_byte_num_limit(int byteNum) {
    if (byteNum > MAX_BYTE_NUM) {
        return E_BYTE_NUM_EXCEEDED;
    }
    return E_SUCCESS;
}

static void normalize_dimensions(microarch_tensor_descriptor_t* desc) {
    if (desc->byteNum <= 0) desc->byteNum = 1;
    if (desc->unitNum <= 0) desc->unitNum = 1;
    if (desc->sliceNum <= 0) desc->sliceNum = 1;
    if (desc->planeNum <= 0) desc->planeNum = 1;
    if (desc->cubeNum <= 0) desc->cubeNum = 1;
}

static int adjust_for_constraints(microarch_tensor_descriptor_t* desc, 
                                   const microarch_constraints_t* constraints,
                                   unsigned long long totalBytes) {
    int changed = 0;
    
    if (constraints->maxUnitNum > 0 && desc->unitNum > (int)constraints->maxUnitNum) {
        unsigned long long remaining = totalBytes / desc->unitNum;
        desc->unitNum = constraints->maxUnitNum;
        changed = 1;
    }
    
    if (constraints->maxSliceNum > 0 && desc->sliceNum > (int)constraints->maxSliceNum) {
        desc->sliceNum = constraints->maxSliceNum;
        changed = 1;
    }
    
    if (constraints->maxPlaneNum > 0 && desc->planeNum > (int)constraints->maxPlaneNum) {
        desc->planeNum = constraints->maxPlaneNum;
        changed = 1;
    }
    
    if (constraints->maxCubeNum > 0 && desc->cubeNum > (int)constraints->maxCubeNum) {
        desc->cubeNum = constraints->maxCubeNum;
        changed = 1;
    }
    
    normalize_dimensions(desc);
    
    return changed;
}

static int validate_feasible(const microarch_tensor_descriptor_t* desc, 
                            const microarch_constraints_t* constraints) {
    unsigned long long currentBytes = (unsigned long long)desc->byteNum * desc->unitNum * 
                                      desc->sliceNum * desc->planeNum * desc->cubeNum;
    
    if (constraints->maxTotalBytes > 0 && currentBytes > constraints->maxTotalBytes) {
        return E_OVER_CONSTRAINED;
    }
    
    if (constraints->maxUnitNum > 0 && desc->unitNum > (int)constraints->maxUnitNum) {
        unsigned long long minBytesNeeded = (unsigned long long)constraints->maxUnitNum * 
                                            desc->sliceNum * desc->planeNum * desc->cubeNum;
        if ((unsigned long long)desc->byteNum * 64 < minBytesNeeded) {
            return E_OVER_CONSTRAINED;
        }
    }
    
    return E_SUCCESS;
}

static int calculate_has_gap(const microarch_tensor_descriptor_t* desc) {
    if (desc->byteNum == 0) return 0;
    if (desc->byteNum == 1) return 1;
    
    int actualStride = 1 << (int)ceil(log2((double)desc->byteNum));
    if (actualStride > desc->byteNum) {
        return 1;
    }
    return 0;
}

static int apply_physical_limits(int* dim, const microarch_physical_limits_t* limits) {
    int error = 0;
    
    if (limits && limits->maxPhysicalByteNum > 0 && dim[0] > (int)limits->maxPhysicalByteNum) {
        error = 1;
    }
    if (limits && limits->maxPhysicalUnitNum > 0 && dim[1] > (int)limits->maxPhysicalUnitNum) {
        error = 1;
    }
    if (limits && limits->maxPhysicalSliceNum > 0 && dim[2] > (int)limits->maxPhysicalSliceNum) {
        error = 1;
    }
    if (limits && limits->maxPhysicalPlaneNum > 0 && dim[3] > (int)limits->maxPhysicalPlaneNum) {
        error = 1;
    }
    if (limits && limits->maxPhysicalCubeNum > 0 && dim[4] > (int)limits->maxPhysicalCubeNum) {
        error = 1;
    }
    
    return error ? E_PHYSICAL_CONSTRAINT : E_SUCCESS;
}

static void expand_dimensions(int* dim, unsigned long long totalBytes) {
    if (dim[0] <= MAX_BYTE_NUM) return;
    
    int extraBytes = dim[0] - MAX_BYTE_NUM;
    int extraUnits = (extraBytes + MAX_BYTE_NUM - 1) / MAX_BYTE_NUM;
    
    dim[1] = dim[1] * (1 + extraUnits);
    dim[0] = MAX_BYTE_NUM;
    
    totalBytes = (unsigned long long)dim[0] * dim[1] * dim[2] * dim[3] * dim[4];
}

int microarch_constraints_convert(const microarch_tensor_descriptor_t* archDesc,
                                  const microarch_constraints_t* constraints,
                                  const microarch_physical_limits_t* physicalLimits,
                                  microarch_conversion_result_t* result) {
    if (!archDesc || !result) {
        if (result) result->errorCode = E_INVALID_DIMENSION;
        return E_INVALID_DIMENSION;
    }
    
    unsigned long long totalBytes = (unsigned long long)archDesc->byteNum * archDesc->unitNum * 
                                     archDesc->sliceNum * archDesc->planeNum * archDesc->cubeNum;
    
    result->desc.baseAddr = archDesc->baseAddr;
    result->desc.byteNum = archDesc->byteNum;
    result->desc.unitNum = archDesc->unitNum;
    result->desc.sliceNum = archDesc->sliceNum;
    result->desc.planeNum = archDesc->planeNum;
    result->desc.cubeNum = archDesc->cubeNum;
    
    expand_dimensions((int*)&result->desc, totalBytes);
    normalize_dimensions(&result->desc);
    
    if (apply_physical_limits((int*)&result->desc, physicalLimits) != E_SUCCESS) {
        result->errorCode = E_PHYSICAL_CONSTRAINT;
        return E_PHYSICAL_CONSTRAINT;
    }
    
    if (constraints) {
        unsigned long long newTotalBytes = (unsigned long long)result->desc.byteNum * 
                                            result->desc.unitNum * result->desc.sliceNum * 
                                            result->desc.planeNum * result->desc.cubeNum;
        
        int feasResult = validate_feasible(&result->desc, constraints);
        if (feasResult != E_SUCCESS) {
            result->errorCode = feasResult;
            return feasResult;
        }
        
        adjust_for_constraints(&result->desc, constraints, newTotalBytes);
    }
    
    result->desc.unitSkip = 1 << (int)ceil(log2((double)result->desc.byteNum));
    result->desc.sliceSkip = result->desc.unitNum * result->desc.unitSkip;
    result->desc.planeSkip = result->desc.sliceNum * result->desc.sliceSkip;
    result->desc.cubeSkip = result->desc.planeNum * result->desc.planeSkip;
    
    result->hasGap = calculate_has_gap(&result->desc);
    result->errorCode = E_SUCCESS;
    
    return E_SUCCESS;
}