#ifndef TENSOR_DESCRIPTOR_H
#define TENSOR_DESCRIPTOR_H

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

/* ============================================================================
 * Module A: Architecture Tensor (Internal)
 * ============================================================================ */
typedef struct {
    unsigned int baseAddr;
    unsigned int dimension[NDIM];
    unsigned int stride[NDIM];
} ggml_tensor_descriptor_t;

typedef struct {
    unsigned int coords[NDIM];
    unsigned int range[NDIM];
    unsigned int traversalStride[NDIM];
} sub_ggml_tensor_descriptor_t;

typedef struct ggml_tensor_t ggml_tensor_t;

/* ============================================================================
 * Module B: Microarchitecture Tensor (Internal)
 * ============================================================================ */
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
    unsigned int maxByteNum;
    unsigned int maxUnitNum;
    unsigned int maxSliceNum;
    unsigned int maxPlaneNum;
    unsigned int maxCubeNum;
    unsigned int maxTotalBytes;
    unsigned int enableBalance;
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

typedef struct microarch_tensor_t microarch_tensor_t;

/* Now define the complete ggml_tensor_t */
struct ggml_tensor_t {
    ggml_tensor_descriptor_t tensorDesc;
    sub_ggml_tensor_descriptor_t subTensorDesc;
    microarch_tensor_t* fatherMicroarchTensor;
    microarch_tensor_t* subMicroarchTensor;
};

/* Now define the complete microarch_tensor_t */
struct microarch_tensor_t {
    microarch_tensor_descriptor_t tensorDesc;
    microarch_tensor_descriptor_t subTensorDesc;
};

/* ============================================================================
 * Public API Types
 * ============================================================================ */
typedef struct {
    unsigned int baseAddr;
    unsigned int dimension[NDIM];
    unsigned int stride[NDIM];
} tensor_descriptor_t;

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
} tensor_conversion_descriptor_t;

typedef struct {
    tensor_conversion_descriptor_t desc;
    int hasGap;
    int errorCode;
    unsigned int effectiveMaxByte;
    unsigned int effectiveMaxUnit;
    unsigned int effectiveMaxSlice;
    unsigned int effectiveMaxPlane;
    unsigned int effectiveMaxCube;
} tensor_conversion_result_t;

typedef struct {
    unsigned int maxByteNum;
    unsigned int maxUnitNum;
    unsigned int maxSliceNum;
    unsigned int maxPlaneNum;
    unsigned int maxCubeNum;
    unsigned int maxTotalBytes;
    unsigned int enableBalance;
} tensor_constraints_t;

typedef struct {
    unsigned int maxPhysicalByteNum;
    unsigned int maxPhysicalUnitNum;
    unsigned int maxPhysicalSliceNum;
    unsigned int maxPhysicalPlaneNum;
    unsigned int maxPhysicalCubeNum;
} tensor_physical_limits_t;

/* ============================================================================
 * Internal Functions (not for direct external use)
 * ============================================================================ */
int microarch_constraints_convert(const unsigned int* archDim,
                                   const unsigned int* archStride,
                                   unsigned int baseAddr,
                                   const microarch_constraints_t* constraints,
                                   const microarch_physical_limits_t* physicalLimits,
                                   microarch_conversion_result_t* result);

/* ============================================================================
 * Public API
 * ============================================================================ */
int tensor_descriptor_convert(const tensor_descriptor_t* desc,
                               const tensor_constraints_t* constraints,
                               const tensor_physical_limits_t* limits,
                               tensor_conversion_result_t* result);

int tensor_descriptor_convert_sub(const tensor_descriptor_t* desc,
                                   const tensor_constraints_t* constraints,
                                   const tensor_physical_limits_t* limits,
                                   tensor_conversion_result_t* result);

#endif
