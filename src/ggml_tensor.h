/* GGML Tensor Traversal Library
 * 
 * This is an independent implementation of GGML tensor traversal functionality.
 * It does NOT depend on llama.cpp source code.
 * 
 * Type definitions are manually extracted from llama.cpp/ggml to ensure compatibility
 * while maintaining zero external dependencies.
 */

#ifndef GGML_TENSOR_H
#define GGML_TENSOR_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * GGML Type Definitions (extracted from llama.cpp ggml.h)
 * These values must match llama.cpp's enum ggml_type
 * ============================================================================ */
typedef enum {
    GGML_TYPE_F32  = 0,
    GGML_TYPE_F16  = 1,
    GGML_TYPE_Q4_0 = 2,
    GGML_TYPE_Q4_1 = 3,
    GGML_TYPE_Q5_0 = 6,
    GGML_TYPE_Q5_1 = 7,
    GGML_TYPE_Q8_0 = 8,
    GGML_TYPE_Q8_1 = 9,
    GGML_TYPE_COUNT = 41,  // Keep array sizes in sync
} ggml_type_t;

/* Number of dimensions in GGML tensors */
#define GGML_MAX_DIMS 4

/* ============================================================================
 * Tensor Descriptor
 * Simplified representation of struct ggml_tensor for traversal purposes
 * ============================================================================ */
typedef struct {
    int64_t ne[GGML_MAX_DIMS];    /* Number of elements in each dimension */
    size_t  nb[GGML_MAX_DIMS];    /* Stride in bytes for each dimension */
    int     type;                 /* ggml_type_t value */
    int     type_size;            /* Bytes per block (ggml_type_size) */
    int     blck_size;            /* Elements per block (ggml_blck_size) */
    void*   data;                 /* Pointer to tensor data */
} ggml_tensor_desc_t;

/* ============================================================================
 * Traversal Item
 * Represents a single block during tensor traversal
 * ============================================================================ */
typedef struct {
    int      block_idx;      /* Sequential block index */
    void*    block_addr;     /* Block starting address (absolute) */
    int      elem_start;     /* Starting element index within tensor */
    int      elem_count;     /* Number of elements in this block */
} ggml_traversal_item_t;

/* ============================================================================
 * Type Information Functions
 * ============================================================================ */

/* Get the size in bytes for a given type
 * Returns bytes per block for quantized types, bytes per element otherwise
 */
int ggml_type_get_size(int type);

/* Get the block size (number of elements per block) for a given type
 * Returns 1 for non-quantized types (F32, F16)
 */
int ggml_type_get_blck_size(int type);

/* ============================================================================
 * Traversal Functions
 * ============================================================================ */

/* Calculate the total number of blocks to traverse
 * Formula: (ne[0]/blck_size) * ne[1] * ne[2] * ne[3]
 */
int ggml_tensor_get_traversal_count(const ggml_tensor_desc_t* desc);

/* Traverse tensor block by block
 * 
 * Returns a malloc'd array of traversal items.
 * Caller is responsible for freeing the returned array.
 * 
 * The traversal follows 4D nested loop order:
 *   for i3 in 0..ne[3]-1
 *     for i2 in 0..ne[2]-1
 *       for i1 in 0..ne[1]-1
 *         for i0 in 0..(ne[0]/blck_size)-1
 *           addr = data + i0*nb[0] + i1*nb[1] + i2*nb[2] + i3*nb[3]
 *
 * Parameters:
 *   desc  - Tensor descriptor
 *   count - Output parameter, receives number of items
 *
 * Returns:
 *   malloc'd array of ggml_traversal_item_t, or NULL on error
 */
ggml_traversal_item_t* ggml_tensor_traversal(const ggml_tensor_desc_t* desc, int* count);

#ifdef __cplusplus
}
#endif

#endif /* GGML_TENSOR_H */
