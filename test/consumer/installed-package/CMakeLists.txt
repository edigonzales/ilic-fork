cmake_minimum_required(VERSION 3.20)
project(ilic_installed_consumer LANGUAGES C CXX)

enable_testing()

find_package(ilic CONFIG REQUIRED)

if(NOT TARGET ilic::core)
    message(FATAL_ERROR "Installed package does not provide ilic::core")
endif()
if(NOT TARGET ilic::capi)
    message(FATAL_ERROR "Installed package does not provide ilic::capi")
endif()

add_executable(cpp_consumer cpp-main.cpp)
target_link_libraries(cpp_consumer PRIVATE ilic::core)
add_test(NAME cpp_consumer COMMAND cpp_consumer)

add_executable(c_consumer c-main.c)
set_target_properties(c_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(c_consumer PRIVATE ilic::capi)
add_test(NAME c_consumer COMMAND c_consumer)
