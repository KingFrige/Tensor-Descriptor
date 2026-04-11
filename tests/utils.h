#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include "tensor_descriptor.h"

#define NDIM 5

unsigned int next_power_of_2(unsigned int x);

microarch_tensor_descriptor_t* microarch_tensor_create_random(void);
microarch_tensor_descriptor_t* microarch_tensor_create_from_desc(const microarch_tensor_descriptor_t* desc);
void microarch_tensor_destroy(microarch_tensor_descriptor_t* tensor);
void microarch_tensor_print(const char* name, const microarch_tensor_descriptor_t* desc);
int microarch_tensor_get_traversal_count(const microarch_tensor_descriptor_t* desc);
int* microarch_tensor_traversal(const microarch_tensor_descriptor_t* myTensorDesc);

#endif