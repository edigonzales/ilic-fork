cmake_minimum_required(VERSION 3.20)
project(ilic_source_consumer LANGUAGES CXX)

set(ILIC_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(ILIC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ILIC_ENABLE_NATIVE_REPOSITORY OFF CACHE BOOL "" FORCE)

add_subdirectory("${ILIC_SOURCE_DIR}" ilic-build)

if(TARGET ilic OR TARGET ilic-format)
    message(FATAL_ERROR "ilic CLI targets must not exist in library-only consumer mode")
endif()
if(NOT TARGET ilic::core)
    message(FATAL_ERROR "ilic::core target is missing")
endif()

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE ilic::core)
