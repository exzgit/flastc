cmake_minimum_required(VERSION 3.22)
project(Flast VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Get LLVM components
llvm_map_components_to_libnames(llvm_libs 
    support core irreader analysis executionengine 
    instcombine object runtimedyld scalaropts 
    transformutils codegen target asmparser asmprinter
    native
)

# Add source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.h"
)

# Create executable
add_executable(flastc ${SOURCES})

# Link LLVM libraries and filesystem
target_link_libraries(flastc ${llvm_libs})

# Link filesystem library for C++17
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(flastc stdc++fs)
endif()

# Include directories
target_include_directories(flastc PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LLVM_INCLUDE_DIRS}
) 