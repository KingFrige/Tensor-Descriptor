#include "microarch_tensor.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    microarch_tensor_descriptor_t desc = {
        .baseAddr  = 0,
        .byteNum   = 64,
        .unitNum   = 28,
        .sliceNum  = 4,
        .planeNum  = 28,
        .cubeNum   = 1,
        .unitSkip  = 64,
        .sliceSkip = 1792,
        .planeSkip = 7168,
        .cubeSkip  = 0
    };

    microarch_tensor_t* myTensor = microarch_tensor_create_from_desc(&desc);
    if (!myTensor) {
        fprintf(stderr, "Failed to create tensor\n");
        return 1;
    }

    int* traversalAddr = microarch_tensor_traversal(&myTensor->tensorDesc);
    int tensorAddrPointNum = microarch_tensor_get_traversal_count(&myTensor->tensorDesc);
    printf("tensorAddrPointNum: %d\n", tensorAddrPointNum);

    free(traversalAddr);
    microarch_tensor_destroy(myTensor);

    return 0;
}