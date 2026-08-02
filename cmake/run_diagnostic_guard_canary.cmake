execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env node
        "${ILIC_SOURCE_DIR}/scripts/check-diagnostic-architecture.mjs"
        "${ILIC_SOURCE_DIR}" --canary
    RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr
)
if(result EQUAL 0)
    message(FATAL_ERROR "Diagnostic architecture guard canary unexpectedly passed")
endif()
message(STATUS "Diagnostic architecture guard canary rejected as expected")
