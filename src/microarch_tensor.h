#ifndef MICROARCH_TENSOR_H
#define MICROARCH_TENSOR_H

#include <stddef.h>

#define NDIM 5

#define MAX_BYTE_NUM 64
#define DEFAULT_MAX_UNIT   1024
#define DEFAULT_MAX_SLICE 1024
#define DEFAULT_MAX_PLANE  1024
#define DEFAULT_MAX_CUBE   1024

#define E_SUCCESS              0
#define E_BYTE_NUM_EXCEEDED  -1
#define E_OVER_CONSTRAINED   -2
#define E_INVALID_DIMENSION  -3
#define E_PHYSICAL_CONSTRAINT -4

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

typedef struct {
    unsigned int maxUnitNum;
    unsigned int maxSliceNum;
    unsigned int maxPlaneNum;
    unsigned int maxCubeNum;
    unsigned int maxTotalBytes;
} microarch_constraints_t;

typedef struct {
    unsigned int maxPhysicalByteNum;
    unsigned int maxPhysicalUnitNum;
    unsigned int maxPhysicalSliceNum;
    unsigned int maxPhysicalPlaneNum;
    unsigned int maxPhysicalCubeNum;
} microarch_physical_limits_t;

typedef struct {
    microarch_tensor_descriptor_t desc;
    int hasGap;
    int errorCode;
    unsigned int effectiveMaxByte;
    unsigned int effectiveMaxUnit;
    unsigned int effectiveMaxSlice;
    unsigned int effectiveMaxPlane;
    unsigned int effectiveMaxCube;
} microarch_conversion_result_t;

microarch_tensor_t* microarch_tensor_create_random(void);
microarch_tensor_t* microarch_tensor_create_from_desc(const microarch_tensor_descriptor_t* desc);
void microarch_tensor_destroy(microarch_tensor_t* tensor);
void microarch_tensor_gen_sub_tensor(microarch_tensor_t* tensor);
int microarch_tensor_get_traversal_count(const microarch_tensor_descriptor_t* desc);
int* microarch_tensor_traversal(const microarch_tensor_descriptor_t* myTensorDesc);
void microarch_tensor_print(const char* name, const microarch_tensor_descriptor_t* desc);

int microarch_constraints_convert(const unsigned int* archDim,
                                  const unsigned int* archStride,
                                  unsigned int baseAddr,
                                  const microarch_constraints_t* constraints,
                                  const microarch_physical_limits_t* physicalLimits,
                                  microarch_conversion_result_t* result);

#endif