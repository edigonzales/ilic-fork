if(NOT DEFINED ILIC OR NOT DEFINED INPUT OR NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR "ILIC, INPUT, and OUTPUT_FILE are required")
endif()

file(REMOVE "${OUTPUT_FILE}")
execute_process(
    COMMAND "${ILIC}" "${INPUT}" -o24 "${OUTPUT_FILE}" -silent
    RESULT_VARIABLE generate_result
)
if(NOT generate_result EQUAL 0 OR NOT EXISTS "${OUTPUT_FILE}")
    message(FATAL_ERROR "ilic failed to generate domain-constraint roundtrip output")
endif()

file(READ "${OUTPUT_FILE}" output)
if(NOT output MATCHES "Code = TEXT\\*20 CONSTRAINTS Nonempty: THIS != \"\";")
    message(FATAL_ERROR "roundtrip output lost or changed the domain constraint")
endif()

execute_process(
    COMMAND "${ILIC}" "${OUTPUT_FILE}" -silent
    RESULT_VARIABLE compile_result
)
if(NOT compile_result EQUAL 0)
    message(FATAL_ERROR "ilic rejected its domain-constraint roundtrip output")
endif()
