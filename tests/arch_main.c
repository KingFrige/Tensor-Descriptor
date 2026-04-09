#include "arch_tensor.h"
#include <stdio.h>

int main(void) {
    arch_tensor_t* myTensor = arch_tensor_create_random();
    if (!myTensor) {
        fprintf(stderr, "Failed to create tensor\n");
        return 1;
    }

    arch_tensor_convert_to_microarch(myTensor);
    if (myTensor->fatherMicroarchTensor) {
        microarch_tensor_traversal(&myTensor->fatherMicroarchTensor->tensorDesc);
    }

    arch_tensor_gen_sub_tensor(myTensor);
    arch_tensor_convert_sub_to_microarch(myTensor);
    if (myTensor->subMicroarchTensor) {
        microarch_tensor_traversal(&myTensor->subMicroarchTensor->tensorDesc);
    }

    arch_tensor_destroy(myTensor);

    return 0;
}