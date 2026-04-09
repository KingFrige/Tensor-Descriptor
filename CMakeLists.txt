cmake_minimum_required(VERSION 3.0.0)

# set the project name
project(tensor-descriptor)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(ext/nlohmann_json ${CMAKE_BINARY_DIR}/nlohmann_json)

add_executable(archTensorDescriptor tests/arch_main.cc)
target_include_directories(archTensorDescriptor PRIVATE src)
target_compile_options(archTensorDescriptor PRIVATE -Wall -Wextra -g -O0 -ggdb)

add_executable(microarchTensorDescriptor tests/microarch_main.cc)
target_include_directories(microarchTensorDescriptor PRIVATE src)
target_compile_options(microarchTensorDescriptor PRIVATE -Wall -Wextra -g -O0 -ggdb)

add_executable(microarchTool tests/microarch_tool.cc)
target_include_directories(microarchTool PRIVATE src)
target_link_libraries(microarchTool PRIVATE nlohmann_json)
target_compile_options(microarchTool PRIVATE -Wall -Wextra -g -O0 -ggdb)
