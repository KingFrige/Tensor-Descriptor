cmake_minimum_required(VERSION 3.0.0)

# set the project name
project(tensor-descriptor)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(archTensorDescriptor src/arch_main.cc)
target_compile_options(archTensorDescriptor PRIVATE -Wall -Wextra -g -O0 -ggdb)

add_executable(microarchTensorDescriptor src/microarch_main.cc)
target_compile_options(microarchTensorDescriptor PRIVATE -Wall -Wextra -g -O0 -ggdb)
