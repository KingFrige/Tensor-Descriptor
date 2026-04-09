cmake_minimum_required(VERSION 3.0.0)

project(tensor-descriptor)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_library(tensor_descriptor_lib src/microarch_tensor.c src/arch_tensor.c)
target_include_directories(tensor_descriptor_lib PUBLIC src)
target_link_libraries(tensor_descriptor_lib PUBLIC m)

add_executable(archTensorDescriptor tests/arch_main.c)
target_link_libraries(archTensorDescriptor PRIVATE tensor_descriptor_lib)
target_include_directories(archTensorDescriptor PRIVATE src)
target_compile_options(archTensorDescriptor PRIVATE -Wall -Wextra -g -O0 -ggdb)

add_executable(microarchTensorDescriptor tests/microarch_main.c)
target_link_libraries(microarchTensorDescriptor PRIVATE tensor_descriptor_lib)
target_include_directories(microarchTensorDescriptor PRIVATE src)
target_compile_options(microarchTensorDescriptor PRIVATE -Wall -Wextra -g -O0 -ggdb)

add_executable(microarchTool tests/microarch_tool.c)
target_link_libraries(microarchTool PRIVATE tensor_descriptor_lib)
target_include_directories(microarchTool PRIVATE src)
target_compile_options(microarchTool PRIVATE -Wall -Wextra -g -O0 -ggdb)