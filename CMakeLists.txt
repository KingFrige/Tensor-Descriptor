cmake_minimum_required(VERSION 3.0.0)

project(tensor-descriptor)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_library(tensor_descriptor_lib src/tensor_descriptor.c)
target_include_directories(tensor_descriptor_lib PUBLIC src)

add_executable(constraint_test tests/constraint_test.c tests/utils.c)
target_link_libraries(constraint_test PRIVATE tensor_descriptor_lib)
target_include_directories(constraint_test PRIVATE src)
target_compile_options(constraint_test PRIVATE -Wall -Wextra -g -O0 -ggdb)

add_executable(batch_constraint_test tests/batch_constraint_test.c tests/utils.c)
target_link_libraries(batch_constraint_test PRIVATE tensor_descriptor_lib)
target_include_directories(batch_constraint_test PRIVATE src)
target_compile_options(batch_constraint_test PRIVATE -Wall -Wextra -g -O0 -ggdb)

# GGML Tensor Traversal Test
add_executable(ggml_tensor_traversal_test tests/ggml_tensor_traversal_test.c src/ggml_tensor.c)
target_include_directories(ggml_tensor_traversal_test PRIVATE src tests)
target_compile_options(ggml_tensor_traversal_test PRIVATE -Wall -Wextra -g -O0 -ggdb)
