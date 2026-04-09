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

arch_tensor_t* arch_tensor_create_random(void) {
    arch_tensor_t* tensor = (arch_tensor_t*)malloc(sizeof(arch_tensor_t));
    if (!tensor) return NULL;

    tensor->tensorDesc.baseAddr  = gen_random_data(100) * 64;
    tensor->tensorDesc.dimension[0] = gen_random_data(64);
    tensor->tensorDesc.dimension[1] = gen_random_data(40);
    tensor->tensorDesc.dimension[2] = gen_random_data(10);
    tensor->tensorDesc.dimension[3] = gen_random_data(20);
    tensor->tensorDesc.dimension[4] = gen_random_data(6);

    tensor->tensorDesc.stride[0] = 1 << (int)ceil(log2((double)tensor->tensorDesc.dimension[0]));
    tensor->tensorDesc.stride[1] = tensor->tensorDesc.dimension[1] * tensor->tensorDesc.stride[0];
    tensor->tensorDesc.stride[2] = tensor->tensorDesc.dimension[2] * tensor->tensorDesc.stride[1];
    tensor->tensorDesc.stride[3] = tensor->tensorDesc.dimension[3] * tensor->tensorDesc.stride[2];
    tensor->tensorDesc.stride[4] = tensor->tensorDesc.dimension[4] * tensor->tensorDesc.stride[3];

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

    int base_addr = (int)tensor->tensorDesc.baseAddr;
    int byte_num  = (int)tensor->tensorDesc.dimension[0];
    int unit_num  = (int)tensor->tensorDesc.dimension[1];
    int slice_num = (int)tensor->tensorDesc.dimension[2];
    int plane_num = (int)tensor->tensorDesc.dimension[3];
    int cube_num  = (int)tensor->tensorDesc.dimension[4];

    int unit_skip  = (int)tensor->tensorDesc.stride[0];
    int slice_skip = (int)tensor->tensorDesc.stride[1];
    int plane_skip = (int)tensor->tensorDesc.stride[2];
    int cube_skip  = (int)tensor->tensorDesc.stride[3];

    microarch_tensor_descriptor_t desc;
    desc.baseAddr  = base_addr;
    desc.byteNum   = byte_num;
    desc.unitNum   = unit_num;
    desc.sliceNum  = slice_num;
    desc.planeNum  = plane_num;
    desc.cubeNum   = cube_num;
    desc.unitSkip  = unit_skip;
    desc.sliceSkip = slice_skip;
    desc.planeSkip = plane_skip;
    desc.cubeSkip  = cube_skip;

    tensor->fatherMicroarchTensor = microarch_tensor_create_from_desc(&desc);
}

void arch_tensor_convert_sub_to_microarch(arch_tensor_t* tensor) {
    if (!tensor) return;

    int base_addr = (int)(tensor->subTensorDesc.coords[0] * tensor->tensorDesc.stride[0] +
                         tensor->subTensorDesc.coords[1] * tensor->tensorDesc.stride[1] +
                         tensor->subTensorDesc.coords[2] * tensor->tensorDesc.stride[2] +
                         tensor->subTensorDesc.coords[3] * tensor->tensorDesc.stride[3] +
                         tensor->subTensorDesc.coords[4] * tensor->tensorDesc.stride[4]);

    int byte_num  = (int)tensor->subTensorDesc.range[0];
    int unit_num  = (int)tensor->subTensorDesc.range[1];
    int slice_num = (int)tensor->subTensorDesc.range[2];
    int plane_num = (int)tensor->subTensorDesc.range[3];
    int cube_num  = (int)tensor->subTensorDesc.range[4];

    int unit_skip  = (int)(tensor->tensorDesc.stride[0] * tensor->subTensorDesc.traversalStride[0]);
    int slice_skip = (int)(tensor->tensorDesc.stride[1] * tensor->subTensorDesc.traversalStride[1]);
    int plane_skip = (int)(tensor->tensorDesc.stride[2] * tensor->subTensorDesc.traversalStride[2]);
    int cube_skip  = (int)(tensor->tensorDesc.stride[3] * tensor->subTensorDesc.traversalStride[3]);

    microarch_tensor_descriptor_t desc;
    desc.baseAddr  = base_addr;
    desc.byteNum   = byte_num;
    desc.unitNum   = unit_num;
    desc.sliceNum  = slice_num;
    desc.planeNum  = plane_num;
    desc.cubeNum   = cube_num;
    desc.unitSkip  = unit_skip;
    desc.sliceSkip = slice_skip;
    desc.planeSkip = plane_skip;
    desc.cubeSkip  = cube_skip;

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