if(NOT DEFINED ILIC OR NOT DEFINED WORK)
    message(FATAL_ERROR "ILIC and WORK are required")
endif()

file(MAKE_DIRECTORY "${WORK}")
set(VALID_MODEL "${WORK}/ValidJsonModel.ili")
set(INVALID_MODEL "${WORK}/InvalidJsonModel.ili")
set(REQUEST "${WORK}/request.json")

file(WRITE "${VALID_MODEL}" [=[INTERLIS 2.3;
MODEL ValidJsonModel AT "https://example.invalid/ilic/compile-json" VERSION "1" =
END ValidJsonModel.
]=])
file(WRITE "${INVALID_MODEL}" [=[INTERLIS 2.3;
MODEL InvalidJsonModel AT "https://example.invalid/ilic/compile-json" VERSION "1" =
]=])

function(run_compile_json model expected_success)
    file(WRITE "${REQUEST}"
        "{\"schemaVersion\":1,\"roots\":[\"${model}\"],\"options\":{\"autoSearch\":true}}")
    execute_process(
        COMMAND "${ILIC}" --compile-json
        INPUT_FILE "${REQUEST}"
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error
        RESULT_VARIABLE status
    )
    if(NOT status EQUAL 0)
        message(FATAL_ERROR "Compilation transport failed with ${status}: ${error}\n${output}")
    endif()
    string(JSON success GET "${output}" success)
    if(NOT success STREQUAL expected_success)
        message(FATAL_ERROR "Expected success=${expected_success}, got ${success}: ${output}")
    endif()
endfunction()

run_compile_json("${VALID_MODEL}" ON)
run_compile_json("${INVALID_MODEL}" OFF)

file(WRITE "${REQUEST}" "{")
execute_process(
    COMMAND "${ILIC}" --compile-json
    INPUT_FILE "${REQUEST}"
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    RESULT_VARIABLE status
)
if(status EQUAL 0)
    message(FATAL_ERROR "Malformed request unexpectedly returned transport success: ${output}")
endif()
string(JSON code GET "${output}" diagnostics 0 code)
if(NOT code STREQUAL "ILIC-ABI-REQUEST")
    message(FATAL_ERROR "Malformed request did not return ILIC-ABI-REQUEST: ${output}")
endif()
