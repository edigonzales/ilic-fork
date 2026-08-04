if(NOT DEFINED ILIC_SOURCE_DIR OR
   NOT DEFINED ILIC_CONSUMER_SOURCE_DIR OR
   NOT DEFINED ILIC_CONSUMER_BUILD_DIR)
    message(FATAL_ERROR "Source-subproject test paths are required")
endif()

set(configure_command
    "${CMAKE_COMMAND}"
    -S "${ILIC_CONSUMER_SOURCE_DIR}"
    -B "${ILIC_CONSUMER_BUILD_DIR}"
    -G "${ILIC_GENERATOR}"
    "-DILIC_SOURCE_DIR=${ILIC_SOURCE_DIR}"
)
if(NOT ILIC_GENERATOR_PLATFORM STREQUAL "")
    list(APPEND configure_command -A "${ILIC_GENERATOR_PLATFORM}")
endif()
if(NOT ILIC_GENERATOR_TOOLSET STREQUAL "")
    list(APPEND configure_command -T "${ILIC_GENERATOR_TOOLSET}")
endif()

file(REMOVE_RECURSE "${ILIC_CONSUMER_BUILD_DIR}")
execute_process(
    COMMAND ${configure_command}
    RESULT_VARIABLE configure_result
    OUTPUT_VARIABLE configure_output
    ERROR_VARIABLE configure_error
)
if(NOT configure_result EQUAL 0)
    message(FATAL_ERROR
        "Source-subproject configure failed:\n${configure_output}\n${configure_error}")
endif()

set(build_command
    "${CMAKE_COMMAND}" --build "${ILIC_CONSUMER_BUILD_DIR}"
    --target consumer --parallel
)
if(NOT ILIC_BUILD_CONFIG STREQUAL "")
    list(APPEND build_command --config "${ILIC_BUILD_CONFIG}")
endif()
execute_process(
    COMMAND ${build_command}
    RESULT_VARIABLE build_result
    OUTPUT_VARIABLE build_output
    ERROR_VARIABLE build_error
)
if(NOT build_result EQUAL 0)
    message(FATAL_ERROR
        "Source-subproject build failed:\n${build_output}\n${build_error}")
endif()

set(consumer_executable
    "${ILIC_CONSUMER_BUILD_DIR}/consumer${ILIC_CONSUMER_EXE_SUFFIX}")
if(NOT EXISTS "${consumer_executable}")
    set(consumer_executable
        "${ILIC_CONSUMER_BUILD_DIR}/${ILIC_BUILD_CONFIG}/consumer${ILIC_CONSUMER_EXE_SUFFIX}")
endif()
if(NOT EXISTS "${consumer_executable}")
    message(FATAL_ERROR "Source-subproject consumer executable was not produced")
endif()

execute_process(
    COMMAND "${consumer_executable}"
    RESULT_VARIABLE run_result
)
if(NOT run_result EQUAL 0)
    message(FATAL_ERROR "Source-subproject consumer exited with ${run_result}")
endif()
