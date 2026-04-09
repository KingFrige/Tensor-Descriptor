#ifndef ARCH_TENSOR_H
#define ARCH_TENSOR_H

#include "microarch_tensor.h"

#define NDIM 5

typedef struct {
    unsigned int baseAddr;
    unsigned int dimension[NDIM];
    unsigned int stride[NDIM];
} arch_tensor_descriptor_t;

typedef struct {
    unsigned int coords[NDIM];
    unsigned int range[NDIM];
    unsigned int traversalStride[NDIM];
} sub_arch_tensor_descriptor_t;

typedef struct {
    arch_tensor_descriptor_t tensorDesc;
    sub_arch_tensor_descriptor_t subTensorDesc;
    microarch_tensor_t* fatherMicroarchTensor;
    microarch_tensor_t* subMicroarchTensor;
} arch_tensor_t;

arch_tensor_t* arch_tensor_create_random(void);
void arch_tensor_destroy(arch_tensor_t* tensor);
void arch_tensor_gen_sub_tensor(arch_tensor_t* tensor);
void arch_tensor_convert_to_microarch(arch_tensor_t* tensor);
void arch_tensor_convert_sub_to_microarch(arch_tensor_t* tensor);
void arch_tensor_print(const char* name, const arch_tensor_descriptor_t* desc);

#endif