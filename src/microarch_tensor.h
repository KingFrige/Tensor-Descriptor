#ifndef MICROARCH_TENSOR_H
#define MICROARCH_TENSOR_H

#include <stddef.h>

#define NDIM 5

typedef struct {
    int baseAddr;
    int byteNum;
    int unitNum;
    int sliceNum;
    int planeNum;
    int cubeNum;
    int unitSkip;
    int sliceSkip;
    int planeSkip;
    int cubeSkip;
} microarch_tensor_descriptor_t;

typedef struct {
    microarch_tensor_descriptor_t tensorDesc;
    microarch_tensor_descriptor_t subTensorDesc;
} microarch_tensor_t;

microarch_tensor_t* microarch_tensor_create_random(void);
microarch_tensor_t* microarch_tensor_create_from_desc(const microarch_tensor_descriptor_t* desc);
void microarch_tensor_destroy(microarch_tensor_t* tensor);
void microarch_tensor_gen_sub_tensor(microarch_tensor_t* tensor);
int microarch_tensor_get_traversal_count(const microarch_tensor_descriptor_t* desc);
int* microarch_tensor_traversal(const microarch_tensor_descriptor_t* myTensorDesc);
void microarch_tensor_print(const char* name, const microarch_tensor_descriptor_t* desc);

#endif