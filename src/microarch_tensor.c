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