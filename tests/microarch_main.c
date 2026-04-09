#include "microarch_tensor.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    microarch_tensor_t* myTensor = microarch_tensor_create_random();
    if (!myTensor) {
        fprintf(stderr, "Failed to create tensor\n");
        return 1;
    }

    int* traversalAddr = microarch_tensor_traversal(&myTensor->tensorDesc);
    int tensorAddrPointNum = microarch_tensor_get_traversal_count(&myTensor->tensorDesc);

    microarch_tensor_gen_sub_tensor(myTensor);
    int subTensorAddrPointNum = microarch_tensor_get_traversal_count(&myTensor->subTensorDesc);

    free(traversalAddr);
    microarch_tensor_destroy(myTensor);

    return 0;
}