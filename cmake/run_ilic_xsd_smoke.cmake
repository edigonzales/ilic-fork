cmake_minimum_required(VERSION 3.20)

foreach(required_variable IN ITEMS ILIC OUTPUT_FILE ILIDIR MODEL)
    if(NOT DEFINED ${required_variable})
        message(FATAL_ERROR "Missing required variable: ${required_variable}")
    endif()
endforeach()

file(REMOVE "${OUTPUT_FILE}")

execute_process(
    COMMAND "${ILIC}"
        -oXSD "${OUTPUT_FILE}"
        -ilidirs "${ILIDIR}"
        "${MODEL}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "ilic XSD smoke test failed with exit code ${result}\n"
        "stdout:\n${output}\n"
        "stderr:\n${error_output}"
    )
endif()

if(NOT EXISTS "${OUTPUT_FILE}")
    message(FATAL_ERROR "ilic did not create the expected XSD file: ${OUTPUT_FILE}")
endif()

file(SIZE "${OUTPUT_FILE}" output_size)
if(output_size LESS 1)
    message(FATAL_ERROR "ilic created an empty XSD file: ${OUTPUT_FILE}")
endif()

message(STATUS "ilic created ${OUTPUT_FILE} (${output_size} bytes)")
