#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned int next_power_of_2(unsigned int x) {
    if (x == 0) return 1;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

static unsigned int gen_random_data(unsigned int max_value) {
    srand((unsigned int)time(0));
    return (rand() % max_value) + 1;
}

static int get_skip_addr(int base_addr, int skip) {
    int raw_addr = base_addr + skip;
    int skip_addr = raw_addr & ((1 << 22) - 1);
    return skip_addr;
}

microarch_tensor_descriptor_t* microarch_tensor_create_random(void) {
    microarch_tensor_descriptor_t* tensor = (microarch_tensor_descriptor_t*)malloc(sizeof(microarch_tensor_descriptor_t));
    if (!tensor) return NULL;

    tensor->baseAddr  = gen_random_data(100) * 64;
    tensor->byteNum   = gen_random_data(64);
    tensor->unitNum   = gen_random_data(40);
    tensor->sliceNum  = gen_random_data(10);
    tensor->planeNum  = gen_random_data(20);
    tensor->cubeNum   = gen_random_data(6);

    tensor->unitSkip  = next_power_of_2(tensor->byteNum);
    tensor->sliceSkip = tensor->unitNum * tensor->unitSkip;
    tensor->planeSkip = tensor->sliceNum * tensor->sliceSkip;
    tensor->cubeSkip  = tensor->planeNum * tensor->planeSkip;

    return tensor;
}

microarch_tensor_descriptor_t* microarch_tensor_create_from_desc(const microarch_tensor_descriptor_t* desc) {
    if (!desc) return NULL;

    microarch_tensor_descriptor_t* tensor = (microarch_tensor_descriptor_t*)malloc(sizeof(microarch_tensor_descriptor_t));
    if (!tensor) return NULL;

    tensor->baseAddr  = desc->baseAddr;
    tensor->byteNum   = desc->byteNum;
    tensor->unitNum   = desc->unitNum;
    tensor->sliceNum  = desc->sliceNum;
    tensor->planeNum  = desc->planeNum;
    tensor->cubeNum   = desc->cubeNum;
    tensor->unitSkip  = desc->unitSkip;
    tensor->sliceSkip = desc->sliceSkip;
    tensor->planeSkip = desc->planeSkip;
    tensor->cubeSkip  = desc->cubeSkip;

    return tensor;
}

void microarch_tensor_destroy(microarch_tensor_descriptor_t* tensor) {
    if (tensor) {
        free(tensor);
    }
}

void microarch_tensor_print(const char* name, const microarch_tensor_descriptor_t* desc) {
    if (!desc) return;
    printf("name: %s\n", name);
    printf("baseAddr: %d\n", desc->baseAddr);
    printf("byteNum: %d, unitNum: %d, sliceNum: %d, planeNum: %d, cubeNum: %d\n",
           desc->byteNum, desc->unitNum, desc->sliceNum, desc->planeNum, desc->cubeNum);
    printf("unitSkip : %d\n", desc->unitSkip);
    printf("sliceSkip: %d\n", desc->sliceSkip);
    printf("planeSkip: %d\n", desc->planeSkip);
    printf("cubeSkip:  %d\n", desc->cubeSkip);
    printf("\n");
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