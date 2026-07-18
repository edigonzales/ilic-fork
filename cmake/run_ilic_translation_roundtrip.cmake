if(NOT DEFINED ILIC OR NOT DEFINED INPUT OR NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR "ILIC, INPUT, and OUTPUT_FILE are required")
endif()

file(REMOVE "${OUTPUT_FILE}")
execute_process(
    COMMAND "${ILIC}" "${INPUT}" -o23 "${OUTPUT_FILE}" -silent
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "ilic roundtrip failed with status ${result}")
endif()
if(NOT EXISTS "${OUTPUT_FILE}")
    message(FATAL_ERROR "ilic did not create ${OUTPUT_FILE}")
endif()
file(READ "${OUTPUT_FILE}" output)
if(NOT output MATCHES "TRANSLATION OF BaseModel \\[\"documented-base-version\"\\]")
    message(FATAL_ERROR "roundtrip output lost or changed the TRANSLATION OF clause")
endif()
