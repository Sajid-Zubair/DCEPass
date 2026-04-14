cmake_minimum_required(VERSION 3.13)
project(DCEPass)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Properly include LLVM headers
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Build plugin
add_library(DCEPass SHARED DCEPass.cpp)

# Link LLVM (IMPORTANT)
target_link_libraries(DCEPass PRIVATE LLVM)