cmake_minimum_required(VERSION 3.20)
project(ilic_source_consumer LANGUAGES CXX)

# Prove that ilic::core raises a consumer's older default to its public C++17
# requirement instead of relying on a directory-scoped CMake variable.
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ILIC_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(ILIC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ILIC_ENABLE_NATIVE_REPOSITORY OFF CACHE BOOL "" FORCE)

add_subdirectory("${ILIC_SOURCE_DIR}" ilic-build)

if(TARGET ilic OR TARGET ilic-format)
    message(FATAL_ERROR "ilic CLI targets must not exist in library-only consumer mode")
endif()
if(ILIC_ENABLE_INSTALL)
    message(FATAL_ERROR
        "ilic install/package rules must stay opt-in for add_subdirectory consumers")
endif()
if(NOT TARGET ilic::core)
    message(FATAL_ERROR "ilic::core target is missing")
endif()
get_target_property(_ilic_compile_features
    ilic-core INTERFACE_COMPILE_FEATURES)
if(NOT "cxx_std_17" IN_LIST _ilic_compile_features)
    message(FATAL_ERROR "ilic::core does not export its C++17 requirement")
endif()

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE ilic::core)
