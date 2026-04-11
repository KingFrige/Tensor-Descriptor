/* Generated test data from qwen_tensor.json */
/* DO NOT EDIT - Run scripts/extract_qwen_tensor.py to regenerate */

#ifndef GGML_TENSOR_TEST_DATA_H
#define GGML_TENSOR_TEST_DATA_H

#include "ggml_tensor.h"

#define GGML_TEST_TENSOR_COUNT 102

/* Tensor info structure for test data */
typedef struct {
    int         id;           /* Unique tensor ID */
    const char* name;         /* Tensor name */
    int         node_id;      /* Node index in graph */
    const char* field;        /* src0, src1, or dst */
    const char* op;           /* Operation name */
    int64_t     ne[4];        /* Dimensions */
    size_t      nb[4];        /* Strides */
    int         type;         /* GGML type */
    int         type_size;    /* Bytes per block */
    int         blck_size;    /* Elements per block */
    int         nbytes;       /* Total bytes */
    int         is_view;      /* Is view tensor */
} ggml_test_tensor_info_t;

static const char ggml_test_name_0[] = "src0";
static const char ggml_test_field_0[] = "src0";
static const char ggml_test_op_0[] = "GET_ROWS";
static const char ggml_test_name_1[] = "src1";
static const char ggml_test_field_1[] = "src1";
static const char ggml_test_op_1[] = "GET_ROWS";
static const char ggml_test_name_2[] = "dst";
static const char ggml_test_field_2[] = "dst";
static const char ggml_test_op_2[] = "GET_ROWS";
static const char ggml_test_name_3[] = "src0";
static const char ggml_test_field_3[] = "src0";
static const char ggml_test_op_3[] = "RMS_NORM";
static const char ggml_test_name_4[] = "dst";
static const char ggml_test_field_4[] = "dst";
static const char ggml_test_op_4[] = "RMS_NORM";
static const char ggml_test_name_5[] = "src0";
static const char ggml_test_field_5[] = "src0";
static const char ggml_test_op_5[] = "MUL";
static const char ggml_test_name_6[] = "src1";
static const char ggml_test_field_6[] = "src1";
static const char ggml_test_op_6[] = "MUL";
static const char ggml_test_name_7[] = "dst";
static const char ggml_test_field_7[] = "dst";
static const char ggml_test_op_7[] = "MUL";
static const char ggml_test_name_8[] = "src0";
static const char ggml_test_field_8[] = "src0";
static const char ggml_test_op_8[] = "MUL_MAT";
static const char ggml_test_name_9[] = "src1";
static const char ggml_test_field_9[] = "src1";
static const char ggml_test_op_9[] = "MUL_MAT";
static const char ggml_test_name_10[] = "dst";
static const char ggml_test_field_10[] = "dst";
static const char ggml_test_op_10[] = "MUL_MAT";
static const char ggml_test_name_11[] = "src0";
static const char ggml_test_field_11[] = "src0";
static const char ggml_test_op_11[] = "RESHAPE";
static const char ggml_test_name_12[] = "dst";
static const char ggml_test_field_12[] = "dst";
static const char ggml_test_op_12[] = "RESHAPE";
static const char ggml_test_name_13[] = "src0";
static const char ggml_test_field_13[] = "src0";
static const char ggml_test_op_13[] = "RMS_NORM";
static const char ggml_test_name_14[] = "dst";
static const char ggml_test_field_14[] = "dst";
static const char ggml_test_op_14[] = "RMS_NORM";
static const char ggml_test_name_15[] = "src0";
static const char ggml_test_field_15[] = "src0";
static const char ggml_test_op_15[] = "MUL";
static const char ggml_test_name_16[] = "src1";
static const char ggml_test_field_16[] = "src1";
static const char ggml_test_op_16[] = "MUL";
static const char ggml_test_name_17[] = "dst";
static const char ggml_test_field_17[] = "dst";
static const char ggml_test_op_17[] = "MUL";
static const char ggml_test_name_18[] = "src0";
static const char ggml_test_field_18[] = "src0";
static const char ggml_test_op_18[] = "ROPE";
static const char ggml_test_name_19[] = "src1";
static const char ggml_test_field_19[] = "src1";
static const char ggml_test_op_19[] = "ROPE";
static const char ggml_test_name_20[] = "dst";
static const char ggml_test_field_20[] = "dst";
static const char ggml_test_op_20[] = "ROPE";
static const char ggml_test_name_21[] = "src0";
static const char ggml_test_field_21[] = "src0";
static const char ggml_test_op_21[] = "MUL_MAT";
static const char ggml_test_name_22[] = "src1";
static const char ggml_test_field_22[] = "src1";
static const char ggml_test_op_22[] = "MUL_MAT";
static const char ggml_test_name_23[] = "dst";
static const char ggml_test_field_23[] = "dst";
static const char ggml_test_op_23[] = "MUL_MAT";
static const char ggml_test_name_24[] = "src0";
static const char ggml_test_field_24[] = "src0";
static const char ggml_test_op_24[] = "RESHAPE";
static const char ggml_test_name_25[] = "dst";
static const char ggml_test_field_25[] = "dst";
static const char ggml_test_op_25[] = "RESHAPE";
static const char ggml_test_name_26[] = "src0";
static const char ggml_test_field_26[] = "src0";
static const char ggml_test_op_26[] = "RMS_NORM";
static const char ggml_test_name_27[] = "dst";
static const char ggml_test_field_27[] = "dst";
static const char ggml_test_op_27[] = "RMS_NORM";
static const char ggml_test_name_28[] = "src0";
static const char ggml_test_field_28[] = "src0";
static const char ggml_test_op_28[] = "MUL";
static const char ggml_test_name_29[] = "src1";
static const char ggml_test_field_29[] = "src1";
static const char ggml_test_op_29[] = "MUL";
static const char ggml_test_name_30[] = "dst";
static const char ggml_test_field_30[] = "dst";
static const char ggml_test_op_30[] = "MUL";
static const char ggml_test_name_31[] = "src0";
static const char ggml_test_field_31[] = "src0";
static const char ggml_test_op_31[] = "ROPE";
static const char ggml_test_name_32[] = "src1";
static const char ggml_test_field_32[] = "src1";
static const char ggml_test_op_32[] = "ROPE";
static const char ggml_test_name_33[] = "dst";
static const char ggml_test_field_33[] = "dst";
static const char ggml_test_op_33[] = "ROPE";
static const char ggml_test_name_34[] = "src0";
static const char ggml_test_field_34[] = "src0";
static const char ggml_test_op_34[] = "MUL_MAT";
static const char ggml_test_name_35[] = "src1";
static const char ggml_test_field_35[] = "src1";
static const char ggml_test_op_35[] = "MUL_MAT";
static const char ggml_test_name_36[] = "dst";
static const char ggml_test_field_36[] = "dst";
static const char ggml_test_op_36[] = "MUL_MAT";
static const char ggml_test_name_37[] = "src0";
static const char ggml_test_field_37[] = "src0";
static const char ggml_test_op_37[] = "RESHAPE";
static const char ggml_test_name_38[] = "dst";
static const char ggml_test_field_38[] = "dst";
static const char ggml_test_op_38[] = "RESHAPE";
static const char ggml_test_name_39[] = "src0";
static const char ggml_test_field_39[] = "src0";
static const char ggml_test_op_39[] = "VIEW";
static const char ggml_test_name_40[] = "dst";
static const char ggml_test_field_40[] = "dst";
static const char ggml_test_op_40[] = "VIEW";
static const char ggml_test_name_41[] = "src0";
static const char ggml_test_field_41[] = "src0";
static const char ggml_test_op_41[] = "CPY";
static const char ggml_test_name_42[] = "dst";
static const char ggml_test_field_42[] = "dst";
static const char ggml_test_op_42[] = "CPY";
static const char ggml_test_name_43[] = "src0";
static const char ggml_test_field_43[] = "src0";
static const char ggml_test_op_43[] = "RESHAPE";
static const char ggml_test_name_44[] = "dst";
static const char ggml_test_field_44[] = "dst";
static const char ggml_test_op_44[] = "RESHAPE";
static const char ggml_test_name_45[] = "src0";
static const char ggml_test_field_45[] = "src0";
static const char ggml_test_op_45[] = "TRANSPOSE";
static const char ggml_test_name_46[] = "dst";
static const char ggml_test_field_46[] = "dst";
static const char ggml_test_op_46[] = "TRANSPOSE";
static const char ggml_test_name_47[] = "src0";
static const char ggml_test_field_47[] = "src0";
static const char ggml_test_op_47[] = "VIEW";
static const char ggml_test_name_48[] = "dst";
static const char ggml_test_field_48[] = "dst";
static const char ggml_test_op_48[] = "VIEW";
static const char ggml_test_name_49[] = "src0";
static const char ggml_test_field_49[] = "src0";
static const char ggml_test_op_49[] = "CPY";
static const char ggml_test_name_50[] = "dst";
static const char ggml_test_field_50[] = "dst";
static const char ggml_test_op_50[] = "CPY";
static const char ggml_test_name_51[] = "src0";
static const char ggml_test_field_51[] = "src0";
static const char ggml_test_op_51[] = "VIEW";
static const char ggml_test_name_52[] = "dst";
static const char ggml_test_field_52[] = "dst";
static const char ggml_test_op_52[] = "VIEW";
static const char ggml_test_name_53[] = "src0";
static const char ggml_test_field_53[] = "src0";
static const char ggml_test_op_53[] = "PERMUTE";
static const char ggml_test_name_54[] = "dst";
static const char ggml_test_field_54[] = "dst";
static const char ggml_test_op_54[] = "PERMUTE";
static const char ggml_test_name_55[] = "src0";
static const char ggml_test_field_55[] = "src0";
static const char ggml_test_op_55[] = "VIEW";
static const char ggml_test_name_56[] = "dst";
static const char ggml_test_field_56[] = "dst";
static const char ggml_test_op_56[] = "VIEW";
static const char ggml_test_name_57[] = "src0";
static const char ggml_test_field_57[] = "src0";
static const char ggml_test_op_57[] = "PERMUTE";
static const char ggml_test_name_58[] = "dst";
static const char ggml_test_field_58[] = "dst";
static const char ggml_test_op_58[] = "PERMUTE";
static const char ggml_test_name_59[] = "src0";
static const char ggml_test_field_59[] = "src0";
static const char ggml_test_op_59[] = "PERMUTE";
static const char ggml_test_name_60[] = "dst";
static const char ggml_test_field_60[] = "dst";
static const char ggml_test_op_60[] = "PERMUTE";
static const char ggml_test_name_61[] = "src0";
static const char ggml_test_field_61[] = "src0";
static const char ggml_test_op_61[] = "MUL_MAT";
static const char ggml_test_name_62[] = "src1";
static const char ggml_test_field_62[] = "src1";
static const char ggml_test_op_62[] = "MUL_MAT";
static const char ggml_test_name_63[] = "dst";
static const char ggml_test_field_63[] = "dst";
static const char ggml_test_op_63[] = "MUL_MAT";
static const char ggml_test_name_64[] = "src0";
static const char ggml_test_field_64[] = "src0";
static const char ggml_test_op_64[] = "SOFT_MAX";
static const char ggml_test_name_65[] = "src1";
static const char ggml_test_field_65[] = "src1";
static const char ggml_test_op_65[] = "SOFT_MAX";
static const char ggml_test_name_66[] = "dst";
static const char ggml_test_field_66[] = "dst";
static const char ggml_test_op_66[] = "SOFT_MAX";
static const char ggml_test_name_67[] = "src0";
static const char ggml_test_field_67[] = "src0";
static const char ggml_test_op_67[] = "MUL_MAT";
static const char ggml_test_name_68[] = "src1";
static const char ggml_test_field_68[] = "src1";
static const char ggml_test_op_68[] = "MUL_MAT";
static const char ggml_test_name_69[] = "dst";
static const char ggml_test_field_69[] = "dst";
static const char ggml_test_op_69[] = "MUL_MAT";
static const char ggml_test_name_70[] = "src0";
static const char ggml_test_field_70[] = "src0";
static const char ggml_test_op_70[] = "PERMUTE";
static const char ggml_test_name_71[] = "dst";
static const char ggml_test_field_71[] = "dst";
static const char ggml_test_op_71[] = "PERMUTE";
static const char ggml_test_name_72[] = "src0";
static const char ggml_test_field_72[] = "src0";
static const char ggml_test_op_72[] = "CONT";
static const char ggml_test_name_73[] = "dst";
static const char ggml_test_field_73[] = "dst";
static const char ggml_test_op_73[] = "CONT";
static const char ggml_test_name_74[] = "src0";
static const char ggml_test_field_74[] = "src0";
static const char ggml_test_op_74[] = "MUL_MAT";
static const char ggml_test_name_75[] = "src1";
static const char ggml_test_field_75[] = "src1";
static const char ggml_test_op_75[] = "MUL_MAT";
static const char ggml_test_name_76[] = "dst";
static const char ggml_test_field_76[] = "dst";
static const char ggml_test_op_76[] = "MUL_MAT";
static const char ggml_test_name_77[] = "src0";
static const char ggml_test_field_77[] = "src0";
static const char ggml_test_op_77[] = "ADD";
static const char ggml_test_name_78[] = "src1";
static const char ggml_test_field_78[] = "src1";
static const char ggml_test_op_78[] = "ADD";
static const char ggml_test_name_79[] = "dst";
static const char ggml_test_field_79[] = "dst";
static const char ggml_test_op_79[] = "ADD";
static const char ggml_test_name_80[] = "src0";
static const char ggml_test_field_80[] = "src0";
static const char ggml_test_op_80[] = "RMS_NORM";
static const char ggml_test_name_81[] = "dst";
static const char ggml_test_field_81[] = "dst";
static const char ggml_test_op_81[] = "RMS_NORM";
static const char ggml_test_name_82[] = "src0";
static const char ggml_test_field_82[] = "src0";
static const char ggml_test_op_82[] = "MUL";
static const char ggml_test_name_83[] = "src1";
static const char ggml_test_field_83[] = "src1";
static const char ggml_test_op_83[] = "MUL";
static const char ggml_test_name_84[] = "dst";
static const char ggml_test_field_84[] = "dst";
static const char ggml_test_op_84[] = "MUL";
static const char ggml_test_name_85[] = "src0";
static const char ggml_test_field_85[] = "src0";
static const char ggml_test_op_85[] = "MUL_MAT";
static const char ggml_test_name_86[] = "src1";
static const char ggml_test_field_86[] = "src1";
static const char ggml_test_op_86[] = "MUL_MAT";
static const char ggml_test_name_87[] = "dst";
static const char ggml_test_field_87[] = "dst";
static const char ggml_test_op_87[] = "MUL_MAT";
static const char ggml_test_name_88[] = "src0";
static const char ggml_test_field_88[] = "src0";
static const char ggml_test_op_88[] = "SILU";
static const char ggml_test_name_89[] = "dst";
static const char ggml_test_field_89[] = "dst";
static const char ggml_test_op_89[] = "SILU";
static const char ggml_test_name_90[] = "src0";
static const char ggml_test_field_90[] = "src0";
static const char ggml_test_op_90[] = "MUL_MAT";
static const char ggml_test_name_91[] = "src1";
static const char ggml_test_field_91[] = "src1";
static const char ggml_test_op_91[] = "MUL_MAT";
static const char ggml_test_name_92[] = "dst";
static const char ggml_test_field_92[] = "dst";
static const char ggml_test_op_92[] = "MUL_MAT";
static const char ggml_test_name_93[] = "src0";
static const char ggml_test_field_93[] = "src0";
static const char ggml_test_op_93[] = "MUL";
static const char ggml_test_name_94[] = "src1";
static const char ggml_test_field_94[] = "src1";
static const char ggml_test_op_94[] = "MUL";
static const char ggml_test_name_95[] = "dst";
static const char ggml_test_field_95[] = "dst";
static const char ggml_test_op_95[] = "MUL";
static const char ggml_test_name_96[] = "src0";
static const char ggml_test_field_96[] = "src0";
static const char ggml_test_op_96[] = "MUL_MAT";
static const char ggml_test_name_97[] = "src1";
static const char ggml_test_field_97[] = "src1";
static const char ggml_test_op_97[] = "MUL_MAT";
static const char ggml_test_name_98[] = "dst";
static const char ggml_test_field_98[] = "dst";
static const char ggml_test_op_98[] = "MUL_MAT";
static const char ggml_test_name_99[] = "src0";
static const char ggml_test_field_99[] = "src0";
static const char ggml_test_op_99[] = "ADD";
static const char ggml_test_name_100[] = "src1";
static const char ggml_test_field_100[] = "src1";
static const char ggml_test_op_100[] = "ADD";
static const char ggml_test_name_101[] = "dst";
static const char ggml_test_field_101[] = "dst";
static const char ggml_test_op_101[] = "ADD";

/* Array of all test tensors */
static const ggml_test_tensor_info_t ggml_test_tensors[] = {
    {
        .id = 0,
        .name = ggml_test_name_0,
        .node_id = 0,
        .field = ggml_test_field_0,
        .op = ggml_test_op_0,
        .ne = {4096, 151936, 1, 1},
        .nb = {18, 2304, 350060544, 350060544},
        .type = 2,
        .type_size = 18,
        .blck_size = 32,
        .nbytes = 350060544,
        .is_view = 0
    },
    {
        .id = 1,
        .name = ggml_test_name_1,
        .node_id = 0,
        .field = ggml_test_field_1,
        .op = ggml_test_op_1,
        .ne = {1, 1, 1, 1},
        .nb = {4, 4, 4, 4},
        .type = 26,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4,
        .is_view = 0
    },
    {
        .id = 2,
        .name = ggml_test_name_2,
        .node_id = 0,
        .field = ggml_test_field_2,
        .op = ggml_test_op_2,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 3,
        .name = ggml_test_name_3,
        .node_id = 1,
        .field = ggml_test_field_3,
        .op = ggml_test_op_3,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 4,
        .name = ggml_test_name_4,
        .node_id = 1,
        .field = ggml_test_field_4,
        .op = ggml_test_op_4,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 5,
        .name = ggml_test_name_5,
        .node_id = 2,
        .field = ggml_test_field_5,
        .op = ggml_test_op_5,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 6,
        .name = ggml_test_name_6,
        .node_id = 2,
        .field = ggml_test_field_6,
        .op = ggml_test_op_6,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 7,
        .name = ggml_test_name_7,
        .node_id = 2,
        .field = ggml_test_field_7,
        .op = ggml_test_op_7,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 8,
        .name = ggml_test_name_8,
        .node_id = 3,
        .field = ggml_test_field_8,
        .op = ggml_test_op_8,
        .ne = {4096, 4096, 1, 1},
        .nb = {18, 2304, 9437184, 9437184},
        .type = 2,
        .type_size = 18,
        .blck_size = 32,
        .nbytes = 9437184,
        .is_view = 0
    },
    {
        .id = 9,
        .name = ggml_test_name_9,
        .node_id = 3,
        .field = ggml_test_field_9,
        .op = ggml_test_op_9,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 10,
        .name = ggml_test_name_10,
        .node_id = 3,
        .field = ggml_test_field_10,
        .op = ggml_test_op_10,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 11,
        .name = ggml_test_name_11,
        .node_id = 4,
        .field = ggml_test_field_11,
        .op = ggml_test_op_11,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 12,
        .name = ggml_test_name_12,
        .node_id = 4,
        .field = ggml_test_field_12,
        .op = ggml_test_op_12,
        .ne = {128, 32, 1, 1},
        .nb = {4, 512, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 1
    },
    {
        .id = 13,
        .name = ggml_test_name_13,
        .node_id = 5,
        .field = ggml_test_field_13,
        .op = ggml_test_op_13,
        .ne = {128, 32, 1, 1},
        .nb = {4, 512, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 1
    },
    {
        .id = 14,
        .name = ggml_test_name_14,
        .node_id = 5,
        .field = ggml_test_field_14,
        .op = ggml_test_op_14,
        .ne = {128, 32, 1, 1},
        .nb = {4, 512, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 15,
        .name = ggml_test_name_15,
        .node_id = 6,
        .field = ggml_test_field_15,
        .op = ggml_test_op_15,
        .ne = {128, 32, 1, 1},
        .nb = {4, 512, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 16,
        .name = ggml_test_name_16,
        .node_id = 6,
        .field = ggml_test_field_16,
        .op = ggml_test_op_16,
        .ne = {128, 1, 1, 1},
        .nb = {4, 512, 512, 512},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 512,
        .is_view = 0
    },
    {
        .id = 17,
        .name = ggml_test_name_17,
        .node_id = 6,
        .field = ggml_test_field_17,
        .op = ggml_test_op_17,
        .ne = {128, 32, 1, 1},
        .nb = {4, 512, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 18,
        .name = ggml_test_name_18,
        .node_id = 7,
        .field = ggml_test_field_18,
        .op = ggml_test_op_18,
        .ne = {128, 32, 1, 1},
        .nb = {4, 512, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 19,
        .name = ggml_test_name_19,
        .node_id = 7,
        .field = ggml_test_field_19,
        .op = ggml_test_op_19,
        .ne = {1, 1, 1, 1},
        .nb = {4, 4, 4, 4},
        .type = 26,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4,
        .is_view = 0
    },
    {
        .id = 20,
        .name = ggml_test_name_20,
        .node_id = 7,
        .field = ggml_test_field_20,
        .op = ggml_test_op_20,
        .ne = {128, 32, 1, 1},
        .nb = {4, 512, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 21,
        .name = ggml_test_name_21,
        .node_id = 8,
        .field = ggml_test_field_21,
        .op = ggml_test_op_21,
        .ne = {4096, 1024, 1, 1},
        .nb = {18, 2304, 2359296, 2359296},
        .type = 2,
        .type_size = 18,
        .blck_size = 32,
        .nbytes = 2359296,
        .is_view = 0
    },
    {
        .id = 22,
        .name = ggml_test_name_22,
        .node_id = 8,
        .field = ggml_test_field_22,
        .op = ggml_test_op_22,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 23,
        .name = ggml_test_name_23,
        .node_id = 8,
        .field = ggml_test_field_23,
        .op = ggml_test_op_23,
        .ne = {1024, 1, 1, 1},
        .nb = {4, 4096, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 24,
        .name = ggml_test_name_24,
        .node_id = 9,
        .field = ggml_test_field_24,
        .op = ggml_test_op_24,
        .ne = {1024, 1, 1, 1},
        .nb = {4, 4096, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 25,
        .name = ggml_test_name_25,
        .node_id = 9,
        .field = ggml_test_field_25,
        .op = ggml_test_op_25,
        .ne = {128, 8, 1, 1},
        .nb = {4, 512, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 1
    },
    {
        .id = 26,
        .name = ggml_test_name_26,
        .node_id = 10,
        .field = ggml_test_field_26,
        .op = ggml_test_op_26,
        .ne = {128, 8, 1, 1},
        .nb = {4, 512, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 1
    },
    {
        .id = 27,
        .name = ggml_test_name_27,
        .node_id = 10,
        .field = ggml_test_field_27,
        .op = ggml_test_op_27,
        .ne = {128, 8, 1, 1},
        .nb = {4, 512, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 28,
        .name = ggml_test_name_28,
        .node_id = 11,
        .field = ggml_test_field_28,
        .op = ggml_test_op_28,
        .ne = {128, 8, 1, 1},
        .nb = {4, 512, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 29,
        .name = ggml_test_name_29,
        .node_id = 11,
        .field = ggml_test_field_29,
        .op = ggml_test_op_29,
        .ne = {128, 1, 1, 1},
        .nb = {4, 512, 512, 512},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 512,
        .is_view = 0
    },
    {
        .id = 30,
        .name = ggml_test_name_30,
        .node_id = 11,
        .field = ggml_test_field_30,
        .op = ggml_test_op_30,
        .ne = {128, 8, 1, 1},
        .nb = {4, 512, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 31,
        .name = ggml_test_name_31,
        .node_id = 12,
        .field = ggml_test_field_31,
        .op = ggml_test_op_31,
        .ne = {128, 8, 1, 1},
        .nb = {4, 512, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 32,
        .name = ggml_test_name_32,
        .node_id = 12,
        .field = ggml_test_field_32,
        .op = ggml_test_op_32,
        .ne = {1, 1, 1, 1},
        .nb = {4, 4, 4, 4},
        .type = 26,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4,
        .is_view = 0
    },
    {
        .id = 33,
        .name = ggml_test_name_33,
        .node_id = 12,
        .field = ggml_test_field_33,
        .op = ggml_test_op_33,
        .ne = {128, 8, 1, 1},
        .nb = {4, 512, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 34,
        .name = ggml_test_name_34,
        .node_id = 13,
        .field = ggml_test_field_34,
        .op = ggml_test_op_34,
        .ne = {4096, 1024, 1, 1},
        .nb = {18, 2304, 2359296, 2359296},
        .type = 2,
        .type_size = 18,
        .blck_size = 32,
        .nbytes = 2359296,
        .is_view = 0
    },
    {
        .id = 35,
        .name = ggml_test_name_35,
        .node_id = 13,
        .field = ggml_test_field_35,
        .op = ggml_test_op_35,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 36,
        .name = ggml_test_name_36,
        .node_id = 13,
        .field = ggml_test_field_36,
        .op = ggml_test_op_36,
        .ne = {1024, 1, 1, 1},
        .nb = {4, 4096, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 37,
        .name = ggml_test_name_37,
        .node_id = 14,
        .field = ggml_test_field_37,
        .op = ggml_test_op_37,
        .ne = {1024, 1, 1, 1},
        .nb = {4, 4096, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 38,
        .name = ggml_test_name_38,
        .node_id = 14,
        .field = ggml_test_field_38,
        .op = ggml_test_op_38,
        .ne = {128, 8, 1, 1},
        .nb = {4, 512, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 1
    },
    {
        .id = 39,
        .name = ggml_test_name_39,
        .node_id = 15,
        .field = ggml_test_field_39,
        .op = ggml_test_op_39,
        .ne = {1024, 4096, 1, 1},
        .nb = {2, 2048, 8388608, 8388608},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8388608,
        .is_view = 0
    },
    {
        .id = 40,
        .name = ggml_test_name_40,
        .node_id = 15,
        .field = ggml_test_field_40,
        .op = ggml_test_op_40,
        .ne = {1024, 1, 1, 1},
        .nb = {2, 2048, 2048, 2048},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 2048,
        .is_view = 1
    },
    {
        .id = 41,
        .name = ggml_test_name_41,
        .node_id = 16,
        .field = ggml_test_field_41,
        .op = ggml_test_op_41,
        .ne = {1024, 4096, 1, 1},
        .nb = {2, 2048, 8388608, 8388608},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8388608,
        .is_view = 0
    },
    {
        .id = 42,
        .name = ggml_test_name_42,
        .node_id = 16,
        .field = ggml_test_field_42,
        .op = ggml_test_op_42,
        .ne = {1024, 1, 1, 1},
        .nb = {2, 2048, 2048, 2048},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 2048,
        .is_view = 1
    },
    {
        .id = 43,
        .name = ggml_test_name_43,
        .node_id = 17,
        .field = ggml_test_field_43,
        .op = ggml_test_op_43,
        .ne = {1024, 1, 1, 1},
        .nb = {4, 4096, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 44,
        .name = ggml_test_name_44,
        .node_id = 17,
        .field = ggml_test_field_44,
        .op = ggml_test_op_44,
        .ne = {1024, 1, 1, 1},
        .nb = {4, 4096, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 1
    },
    {
        .id = 45,
        .name = ggml_test_name_45,
        .node_id = 18,
        .field = ggml_test_field_45,
        .op = ggml_test_op_45,
        .ne = {1024, 1, 1, 1},
        .nb = {4, 4096, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 0
    },
    {
        .id = 46,
        .name = ggml_test_name_46,
        .node_id = 18,
        .field = ggml_test_field_46,
        .op = ggml_test_op_46,
        .ne = {1, 1024, 1, 1},
        .nb = {4096, 4, 4096, 4096},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 4096,
        .is_view = 1
    },
    {
        .id = 47,
        .name = ggml_test_name_47,
        .node_id = 19,
        .field = ggml_test_field_47,
        .op = ggml_test_op_47,
        .ne = {1024, 4096, 1, 1},
        .nb = {2, 2048, 8388608, 8388608},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8388608,
        .is_view = 0
    },
    {
        .id = 48,
        .name = ggml_test_name_48,
        .node_id = 19,
        .field = ggml_test_field_48,
        .op = ggml_test_op_48,
        .ne = {1, 1024, 1, 1},
        .nb = {2, 8192, 8388608, 8388608},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8380418,
        .is_view = 1
    },
    {
        .id = 49,
        .name = ggml_test_name_49,
        .node_id = 20,
        .field = ggml_test_field_49,
        .op = ggml_test_op_49,
        .ne = {1024, 4096, 1, 1},
        .nb = {2, 2048, 8388608, 8388608},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8388608,
        .is_view = 0
    },
    {
        .id = 50,
        .name = ggml_test_name_50,
        .node_id = 20,
        .field = ggml_test_field_50,
        .op = ggml_test_op_50,
        .ne = {1, 1024, 1, 1},
        .nb = {2, 8192, 8388608, 8388608},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8380418,
        .is_view = 1
    },
    {
        .id = 51,
        .name = ggml_test_name_51,
        .node_id = 21,
        .field = ggml_test_field_51,
        .op = ggml_test_op_51,
        .ne = {1024, 4096, 1, 1},
        .nb = {2, 2048, 8388608, 8388608},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8388608,
        .is_view = 0
    },
    {
        .id = 52,
        .name = ggml_test_name_52,
        .node_id = 21,
        .field = ggml_test_field_52,
        .op = ggml_test_op_52,
        .ne = {288, 8, 128, 1},
        .nb = {2, 1048576, 8192, 1048576},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8380992,
        .is_view = 1
    },
    {
        .id = 53,
        .name = ggml_test_name_53,
        .node_id = 22,
        .field = ggml_test_field_53,
        .op = ggml_test_op_53,
        .ne = {1024, 4096, 1, 1},
        .nb = {2, 2048, 8388608, 8388608},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8388608,
        .is_view = 0
    },
    {
        .id = 54,
        .name = ggml_test_name_54,
        .node_id = 22,
        .field = ggml_test_field_54,
        .op = ggml_test_op_54,
        .ne = {288, 128, 8, 1},
        .nb = {2, 8192, 1048576, 1048576},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8380992,
        .is_view = 1
    },
    {
        .id = 55,
        .name = ggml_test_name_55,
        .node_id = 23,
        .field = ggml_test_field_55,
        .op = ggml_test_op_55,
        .ne = {1024, 4096, 1, 1},
        .nb = {2, 2048, 8388608, 8388608},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8388608,
        .is_view = 0
    },
    {
        .id = 56,
        .name = ggml_test_name_56,
        .node_id = 23,
        .field = ggml_test_field_56,
        .op = ggml_test_op_56,
        .ne = {128, 8, 288, 1},
        .nb = {2, 256, 2048, 589824},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 589824,
        .is_view = 1
    },
    {
        .id = 57,
        .name = ggml_test_name_57,
        .node_id = 24,
        .field = ggml_test_field_57,
        .op = ggml_test_op_57,
        .ne = {1024, 4096, 1, 1},
        .nb = {2, 2048, 8388608, 8388608},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8388608,
        .is_view = 0
    },
    {
        .id = 58,
        .name = ggml_test_name_58,
        .node_id = 24,
        .field = ggml_test_field_58,
        .op = ggml_test_op_58,
        .ne = {128, 288, 8, 1},
        .nb = {2, 2048, 256, 589824},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 589824,
        .is_view = 1
    },
    {
        .id = 59,
        .name = ggml_test_name_59,
        .node_id = 25,
        .field = ggml_test_field_59,
        .op = ggml_test_op_59,
        .ne = {128, 32, 1, 1},
        .nb = {4, 512, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 60,
        .name = ggml_test_name_60,
        .node_id = 25,
        .field = ggml_test_field_60,
        .op = ggml_test_op_60,
        .ne = {128, 1, 32, 1},
        .nb = {4, 16384, 512, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 1
    },
    {
        .id = 61,
        .name = ggml_test_name_61,
        .node_id = 26,
        .field = ggml_test_field_61,
        .op = ggml_test_op_61,
        .ne = {128, 288, 8, 1},
        .nb = {2, 2048, 256, 589824},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 589824,
        .is_view = 1
    },
    {
        .id = 62,
        .name = ggml_test_name_62,
        .node_id = 26,
        .field = ggml_test_field_62,
        .op = ggml_test_op_62,
        .ne = {128, 1, 32, 1},
        .nb = {4, 16384, 512, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 1
    },
    {
        .id = 63,
        .name = ggml_test_name_63,
        .node_id = 26,
        .field = ggml_test_field_63,
        .op = ggml_test_op_63,
        .ne = {288, 1, 32, 1},
        .nb = {4, 1152, 1152, 36864},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 36864,
        .is_view = 0
    },
    {
        .id = 64,
        .name = ggml_test_name_64,
        .node_id = 27,
        .field = ggml_test_field_64,
        .op = ggml_test_op_64,
        .ne = {288, 1, 32, 1},
        .nb = {4, 1152, 1152, 36864},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 36864,
        .is_view = 0
    },
    {
        .id = 65,
        .name = ggml_test_name_65,
        .node_id = 27,
        .field = ggml_test_field_65,
        .op = ggml_test_op_65,
        .ne = {288, 64, 1, 1},
        .nb = {4, 1152, 73728, 73728},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 73728,
        .is_view = 0
    },
    {
        .id = 66,
        .name = ggml_test_name_66,
        .node_id = 27,
        .field = ggml_test_field_66,
        .op = ggml_test_op_66,
        .ne = {288, 1, 32, 1},
        .nb = {4, 1152, 1152, 36864},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 36864,
        .is_view = 0
    },
    {
        .id = 67,
        .name = ggml_test_name_67,
        .node_id = 28,
        .field = ggml_test_field_67,
        .op = ggml_test_op_67,
        .ne = {288, 128, 8, 1},
        .nb = {2, 8192, 1048576, 1048576},
        .type = 1,
        .type_size = 2,
        .blck_size = 1,
        .nbytes = 8380992,
        .is_view = 1
    },
    {
        .id = 68,
        .name = ggml_test_name_68,
        .node_id = 28,
        .field = ggml_test_field_68,
        .op = ggml_test_op_68,
        .ne = {288, 1, 32, 1},
        .nb = {4, 1152, 1152, 36864},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 36864,
        .is_view = 0
    },
    {
        .id = 69,
        .name = ggml_test_name_69,
        .node_id = 28,
        .field = ggml_test_field_69,
        .op = ggml_test_op_69,
        .ne = {128, 1, 32, 1},
        .nb = {4, 512, 512, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 70,
        .name = ggml_test_name_70,
        .node_id = 29,
        .field = ggml_test_field_70,
        .op = ggml_test_op_70,
        .ne = {128, 1, 32, 1},
        .nb = {4, 512, 512, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 71,
        .name = ggml_test_name_71,
        .node_id = 29,
        .field = ggml_test_field_71,
        .op = ggml_test_op_71,
        .ne = {128, 32, 1, 1},
        .nb = {4, 512, 512, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 1
    },
    {
        .id = 72,
        .name = ggml_test_name_72,
        .node_id = 30,
        .field = ggml_test_field_72,
        .op = ggml_test_op_72,
        .ne = {128, 32, 1, 1},
        .nb = {4, 512, 512, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 1
    },
    {
        .id = 73,
        .name = ggml_test_name_73,
        .node_id = 30,
        .field = ggml_test_field_73,
        .op = ggml_test_op_73,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 74,
        .name = ggml_test_name_74,
        .node_id = 31,
        .field = ggml_test_field_74,
        .op = ggml_test_op_74,
        .ne = {4096, 4096, 1, 1},
        .nb = {18, 2304, 9437184, 9437184},
        .type = 2,
        .type_size = 18,
        .blck_size = 32,
        .nbytes = 9437184,
        .is_view = 0
    },
    {
        .id = 75,
        .name = ggml_test_name_75,
        .node_id = 31,
        .field = ggml_test_field_75,
        .op = ggml_test_op_75,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 76,
        .name = ggml_test_name_76,
        .node_id = 31,
        .field = ggml_test_field_76,
        .op = ggml_test_op_76,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 77,
        .name = ggml_test_name_77,
        .node_id = 32,
        .field = ggml_test_field_77,
        .op = ggml_test_op_77,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 78,
        .name = ggml_test_name_78,
        .node_id = 32,
        .field = ggml_test_field_78,
        .op = ggml_test_op_78,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 79,
        .name = ggml_test_name_79,
        .node_id = 32,
        .field = ggml_test_field_79,
        .op = ggml_test_op_79,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 80,
        .name = ggml_test_name_80,
        .node_id = 33,
        .field = ggml_test_field_80,
        .op = ggml_test_op_80,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 81,
        .name = ggml_test_name_81,
        .node_id = 33,
        .field = ggml_test_field_81,
        .op = ggml_test_op_81,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 82,
        .name = ggml_test_name_82,
        .node_id = 34,
        .field = ggml_test_field_82,
        .op = ggml_test_op_82,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 83,
        .name = ggml_test_name_83,
        .node_id = 34,
        .field = ggml_test_field_83,
        .op = ggml_test_op_83,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 84,
        .name = ggml_test_name_84,
        .node_id = 34,
        .field = ggml_test_field_84,
        .op = ggml_test_op_84,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 85,
        .name = ggml_test_name_85,
        .node_id = 35,
        .field = ggml_test_field_85,
        .op = ggml_test_op_85,
        .ne = {4096, 12288, 1, 1},
        .nb = {18, 2304, 28311552, 28311552},
        .type = 2,
        .type_size = 18,
        .blck_size = 32,
        .nbytes = 28311552,
        .is_view = 0
    },
    {
        .id = 86,
        .name = ggml_test_name_86,
        .node_id = 35,
        .field = ggml_test_field_86,
        .op = ggml_test_op_86,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 87,
        .name = ggml_test_name_87,
        .node_id = 35,
        .field = ggml_test_field_87,
        .op = ggml_test_op_87,
        .ne = {12288, 1, 1, 1},
        .nb = {4, 49152, 49152, 49152},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 49152,
        .is_view = 0
    },
    {
        .id = 88,
        .name = ggml_test_name_88,
        .node_id = 36,
        .field = ggml_test_field_88,
        .op = ggml_test_op_88,
        .ne = {12288, 1, 1, 1},
        .nb = {4, 49152, 49152, 49152},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 49152,
        .is_view = 0
    },
    {
        .id = 89,
        .name = ggml_test_name_89,
        .node_id = 36,
        .field = ggml_test_field_89,
        .op = ggml_test_op_89,
        .ne = {12288, 1, 1, 1},
        .nb = {4, 49152, 49152, 49152},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 49152,
        .is_view = 0
    },
    {
        .id = 90,
        .name = ggml_test_name_90,
        .node_id = 37,
        .field = ggml_test_field_90,
        .op = ggml_test_op_90,
        .ne = {4096, 12288, 1, 1},
        .nb = {18, 2304, 28311552, 28311552},
        .type = 2,
        .type_size = 18,
        .blck_size = 32,
        .nbytes = 28311552,
        .is_view = 0
    },
    {
        .id = 91,
        .name = ggml_test_name_91,
        .node_id = 37,
        .field = ggml_test_field_91,
        .op = ggml_test_op_91,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 92,
        .name = ggml_test_name_92,
        .node_id = 37,
        .field = ggml_test_field_92,
        .op = ggml_test_op_92,
        .ne = {12288, 1, 1, 1},
        .nb = {4, 49152, 49152, 49152},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 49152,
        .is_view = 0
    },
    {
        .id = 93,
        .name = ggml_test_name_93,
        .node_id = 38,
        .field = ggml_test_field_93,
        .op = ggml_test_op_93,
        .ne = {12288, 1, 1, 1},
        .nb = {4, 49152, 49152, 49152},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 49152,
        .is_view = 0
    },
    {
        .id = 94,
        .name = ggml_test_name_94,
        .node_id = 38,
        .field = ggml_test_field_94,
        .op = ggml_test_op_94,
        .ne = {12288, 1, 1, 1},
        .nb = {4, 49152, 49152, 49152},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 49152,
        .is_view = 0
    },
    {
        .id = 95,
        .name = ggml_test_name_95,
        .node_id = 38,
        .field = ggml_test_field_95,
        .op = ggml_test_op_95,
        .ne = {12288, 1, 1, 1},
        .nb = {4, 49152, 49152, 49152},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 49152,
        .is_view = 0
    },
    {
        .id = 96,
        .name = ggml_test_name_96,
        .node_id = 39,
        .field = ggml_test_field_96,
        .op = ggml_test_op_96,
        .ne = {12288, 4096, 1, 1},
        .nb = {18, 6912, 28311552, 28311552},
        .type = 2,
        .type_size = 18,
        .blck_size = 32,
        .nbytes = 28311552,
        .is_view = 0
    },
    {
        .id = 97,
        .name = ggml_test_name_97,
        .node_id = 39,
        .field = ggml_test_field_97,
        .op = ggml_test_op_97,
        .ne = {12288, 1, 1, 1},
        .nb = {4, 49152, 49152, 49152},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 49152,
        .is_view = 0
    },
    {
        .id = 98,
        .name = ggml_test_name_98,
        .node_id = 39,
        .field = ggml_test_field_98,
        .op = ggml_test_op_98,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 99,
        .name = ggml_test_name_99,
        .node_id = 40,
        .field = ggml_test_field_99,
        .op = ggml_test_op_99,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 100,
        .name = ggml_test_name_100,
        .node_id = 40,
        .field = ggml_test_field_100,
        .op = ggml_test_op_100,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
    {
        .id = 101,
        .name = ggml_test_name_101,
        .node_id = 40,
        .field = ggml_test_field_101,
        .op = ggml_test_op_101,
        .ne = {4096, 1, 1, 1},
        .nb = {4, 16384, 16384, 16384},
        .type = 0,
        .type_size = 4,
        .blck_size = 1,
        .nbytes = 16384,
        .is_view = 0
    },
};

/* Helper: Get tensor descriptor from test array */
static inline void ggml_test_get_tensor_desc(int idx, ggml_tensor_desc_t* desc) {
    if (idx < 0 || idx >= GGML_TEST_TENSOR_COUNT) {
        desc->ne[0]=1;desc->ne[1]=1;desc->ne[2]=1;desc->ne[3]=1;
        desc->nb[0]=0;desc->nb[1]=0;desc->nb[2]=0;desc->nb[3]=0;
        desc->type = 0; desc->type_size = 4; desc->blck_size = 1;
        desc->data = NULL;
        return;
    }
    const ggml_test_tensor_info_t* info = &ggml_test_tensors[idx];
    for (int i = 0; i < 4; i++) {
        desc->ne[i] = info->ne[i];
        desc->nb[i] = info->nb[i];
    }
    desc->type = info->type;
    desc->type_size = info->type_size;
    desc->blck_size = info->blck_size;
    desc->data = NULL; /* Caller must allocate/provide */
}

/* Helper: Get tensor info pointer */
static inline const ggml_test_tensor_info_t* ggml_test_get_info(int idx) {
    if (idx < 0 || idx >= GGML_TEST_TENSOR_COUNT) return NULL;
    return &ggml_test_tensors[idx];
}

#endif /* GGML_TENSOR_TEST_DATA_H */
