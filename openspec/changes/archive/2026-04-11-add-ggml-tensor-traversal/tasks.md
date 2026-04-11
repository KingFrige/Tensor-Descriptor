## 1. Create Header File

- [x] 1.1 Create `src/ggml_tensor.h` with type definitions
  - Define `ggml_type_t` enum (F32=0, F16=1, Q4_0=2, Q8_0=8)
  - Define `ggml_tensor_desc_t` structure
  - Define `ggml_traversal_item_t` structure
  - Declare API functions

## 2. Create Implementation File

- [x] 2.1 Create `src/ggml_tensor.c` with type info tables
  - Implement `ggml_type_get_size()` lookup table
  - Implement `ggml_type_get_blck_size()` lookup table
- [x] 2.2 Implement `ggml_tensor_get_traversal_count()`
  - Calculate total blocks: (ne[0]/blck_size) * ne[1] * ne[2] * ne[3]
- [x] 2.3 Implement `ggml_tensor_traversal()`
  - 4D nested loop over blocks (not elements)
  - Address calculation: data + i0*nb[0] + i1*nb[1] + i2*nb[2] + i3*nb[3]
  - Populate traversal items with block and element info
  - Return malloc'd array

## 3. Create Python Extraction Script

- [x] 3.1 Create `scripts/extract_qwen_tensor.py`
  - Parse `docs/qwen_tensor.json`
  - Extract first few tensor entries
  - Generate C header with hardcoded test data
- [x] 3.2 Run script to generate `tests/ggml_tensor_test_data.inc`

## 4. Create Test Program

- [x] 4.1 Create `tests/ggml_tensor_traversal_test.c`
  - Include generated test data
  - Call `ggml_tensor_traversal()` with test tensor
  - Print traversal results (block addresses)
  - Verify address calculations match expected values
- [x] 4.2 Add test target to `CMakeLists.txt`

## 5. Build and Verify

- [x] 5.1 Run CMake to configure build
- [x] 5.2 Compile without errors
- [x] 5.3 Run test and verify output
- [x] 5.4 Check memory leaks with valgrind (optional) - SKIPPED

## 6. Documentation

- [x] 6.1 Update `TODO.md` to mark task complete
- [x] 6.2 Add brief comment in `src/ggml_tensor.h` about independence from llama.cpp
