if(NOT DEFINED ILIC_BUILD_DIR OR
   NOT DEFINED ILIC_CONSUMER_SOURCE_DIR OR
   NOT DEFINED ILIC_TEST_ROOT)
    message(FATAL_ERROR "Installed-package test paths are required")
endif()

set(install_prefix "${ILIC_TEST_ROOT}/prefix")
set(consumer_build_dir "${ILIC_TEST_ROOT}/consumer-build")
file(REMOVE_RECURSE "${ILIC_TEST_ROOT}")
file(MAKE_DIRECTORY "${ILIC_TEST_ROOT}")

set(install_command
    "${CMAKE_COMMAND}" --install "${ILIC_BUILD_DIR}"
    --prefix "${install_prefix}"
)
if(NOT ILIC_BUILD_CONFIG STREQUAL "")
    list(APPEND install_command --config "${ILIC_BUILD_CONFIG}")
endif()
execute_process(
    COMMAND ${install_command}
    RESULT_VARIABLE install_result
    OUTPUT_VARIABLE install_output
    ERROR_VARIABLE install_error
)
if(NOT install_result EQUAL 0)
    message(FATAL_ERROR
        "ilic install failed:\n${install_output}\n${install_error}")
endif()

set(configure_command
    "${CMAKE_COMMAND}"
    -S "${ILIC_CONSUMER_SOURCE_DIR}"
    -B "${consumer_build_dir}"
    -G "${ILIC_GENERATOR}"
    "-DCMAKE_PREFIX_PATH=${install_prefix}"
)
if(NOT ILIC_GENERATOR_PLATFORM STREQUAL "")
    list(APPEND configure_command -A "${ILIC_GENERATOR_PLATFORM}")
endif()
if(NOT ILIC_GENERATOR_TOOLSET STREQUAL "")
    list(APPEND configure_command -T "${ILIC_GENERATOR_TOOLSET}")
endif()
if(NOT ILIC_BUILD_CONFIG STREQUAL "")
    list(APPEND configure_command "-DCMAKE_BUILD_TYPE=${ILIC_BUILD_CONFIG}")
endif()

execute_process(
    COMMAND ${configure_command}
    RESULT_VARIABLE configure_result
    OUTPUT_VARIABLE configure_output
    ERROR_VARIABLE configure_error
)
if(NOT configure_result EQUAL 0)
    message(FATAL_ERROR
        "Installed-package consumer configure failed:\n${configure_output}\n${configure_error}")
endif()

set(build_command
    "${CMAKE_COMMAND}" --build "${consumer_build_dir}" --parallel
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
        "Installed-package consumer build failed:\n${build_output}\n${build_error}")
endif()

function(run_installed_consumer target_name)
    set(consumer_executable
        "${consumer_build_dir}/${target_name}${ILIC_CONSUMER_EXE_SUFFIX}")
    if(NOT EXISTS "${consumer_executable}" AND
       NOT ILIC_BUILD_CONFIG STREQUAL "")
        set(consumer_executable
            "${consumer_build_dir}/${ILIC_BUILD_CONFIG}/${target_name}${ILIC_CONSUMER_EXE_SUFFIX}")
    endif()
    if(NOT EXISTS "${consumer_executable}")
        message(FATAL_ERROR
            "Installed-package consumer ${target_name} was not produced")
    endif()

    execute_process(
        COMMAND "${consumer_executable}"
        RESULT_VARIABLE run_result
    )
    if(NOT run_result EQUAL 0)
        message(FATAL_ERROR
            "Installed-package consumer ${target_name} exited with ${run_result}")
    endif()
endfunction()

run_installed_consumer(cpp_consumer)
run_installed_consumer(c_consumer)
