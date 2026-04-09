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

static void compute_arch_strides(const unsigned int* dim, unsigned int* stride) {
    stride[0] = 1 << (int)ceil(log2((double)dim[0]));
    for (int i = 1; i < NDIM; i++) {
        stride[i] = dim[i] * stride[i-1];
    }
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

void arch_tensor_print(const char* name, const arch_tensor_descriptor_t* desc) {
    if (!desc) return;
    printf("baseAddr: %u\n", desc->baseAddr);
    printf("dimension[0]:%u, dimension[1]:%u, dimension[2]:%u, dimension[3]:%u, dimension[4]:%u\n",
           desc->dimension[0], desc->dimension[1], desc->dimension[2], desc->dimension[3], desc->dimension[4]);
    printf("stride[0]:%u, stride[1]:%u, stride[2]:%u, stride[3]:%u, stride[4]:%u\n",
           desc->stride[0], desc->stride[1], desc->stride[2], desc->stride[3], desc->stride[4]);
    printf("\n");
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
