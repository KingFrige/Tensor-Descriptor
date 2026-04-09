## 1. Microarchitecture Tensor (C Version)

- [x] 1.1 Create `src/microarch_tensor.h` with structure and function declarations
- [x] 1.2 Define `microarch_tensor_descriptor_t` structure with all 10 fields
- [x] 1.3 Define `microarch_tensor_t` opaque type with internal state
- [x] 1.4 Implement `microarch_tensor_create_random()` with random data generation
- [x] 1.5 Implement `microarch_tensor_create_from_desc()` for parameterized creation
- [x] 1.6 Implement `microarch_tensor_destroy()` for cleanup
- [x] 1.7 Implement `microarch_tensor_gen_sub_tensor()` for sub-tensor generation
- [x] 1.8 Implement `microarch_tensor_get_traversal_count()` calculation
- [x] 1.9 Implement `microarch_tensor_traversal()` for address generation
- [x] 1.10 Add helper functions: `get_skip_addr()`, `string_tensor()` for debugging

## 2. Architecture Tensor (C Version)

- [x] 2.1 Create `src/arch_tensor.h` with structure and function declarations
- [x] 2.2 Define `arch_tensor_descriptor_t` with dimension[5] and stride[5]
- [x] 2.3 Define `sub_arch_tensor_descriptor_t` with coords, range, traversal_stride
- [x] 2.4 Define `arch_tensor_t` opaque type with internal state
- [x] 2.5 Implement `arch_tensor_create_random()` with 5D random generation
- [x] 2.6 Implement `arch_tensor_destroy()` for cleanup
- [x] 2.7 Implement `arch_tensor_gen_sub_tensor()` for sub-tensor coordinate generation
- [x] 2.8 Implement `arch_tensor_convert_to_microarch()` for full tensor conversion
- [x] 2.9 Implement `arch_tensor_convert_sub_to_microarch()` for sub-tensor conversion
- [x] 2.10 Add helper functions: `string_arch_tensor()`, `max()` utility

## 3. Update Build System

- [x] 3.1 Update `CMakeLists.txt` to use C11 standard (`set(CMAKE_C_STANDARD 11)`)
- [x] 3.2 Change source file extensions from `.cc` to `.c` in CMakeLists.txt
- [x] 3.3 Remove `nlohmann_json` dependency from CMakeLists.txt
- [x] 3.4 Remove `target_include_directories` for C++ headers
- [x] 3.5 Add compiler flags for C: `-Wall -Wextra -g -O0`

## 4. Update Test Programs (C Rewrite)

- [x] 4.1 Rewrite `tests/microarch_main.c` (from microarch_main.cc)
- [x] 4.2 Rewrite `tests/arch_main.c` (from arch_main.cc)
- [x] 4.3 Rewrite `tests/microarch_tool.c` (from microarch_tool.cc)
- [x] 4.4 Rename test files in CMakeLists.txt from `.cc` to `.c`
- [x] 4.5 Verify all tests compile without warnings
- [x] 4.6 Verify output format matches C++ version

## 5. Verification and Cleanup

- [x] 5.1 Build project with `cd build && cmake .. && make`
- [x] 5.2 Run `./microarchTensorDescriptor` and verify output
- [x] 5.3 Run `./archTensorDescriptor` and verify output
- [x] 5.4 Delete `configs/demo.json` (no longer needed)
- [x] 5.5 Delete `ext/nlohmann_json/` directory (dependency removed)
- [x] 5.6 Update `TODO.md` to mark C rewrite as complete