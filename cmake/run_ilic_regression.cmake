if(NOT DEFINED ILIC OR ILIC STREQUAL "")
    message(FATAL_ERROR "ILIC must point to the ilic executable")
endif()

if(NOT DEFINED EXPECT_SUCCESS)
    message(FATAL_ERROR "EXPECT_SUCCESS must be TRUE or FALSE")
endif()

if(NOT DEFINED INPUTS OR INPUTS STREQUAL "")
    message(FATAL_ERROR "INPUTS must contain at least one INTERLIS file")
endif()

set(command "${ILIC}")
if(NOT DEFINED OUTPUT_REGEX OR OUTPUT_REGEX STREQUAL "")
    list(APPEND command -silent)
endif()
if(DEFINED ILIDIRS AND NOT ILIDIRS STREQUAL "")
    list(APPEND command -ilidirs "${ILIDIRS}")
endif()
list(APPEND command ${INPUTS})

execute_process(
    COMMAND ${command}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr
)

set(output "${stdout}${stderr}")
if(result MATCHES "[Ss]egmentation|[Aa]bort|[Ee]xception|[Ss]ignal" OR result STREQUAL "139")
    message(FATAL_ERROR
        "ilic terminated abnormally (${result})\n"
        "command: ${command}\n${output}"
    )
endif()

if(EXPECT_SUCCESS)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR
            "ilic unexpectedly rejected a valid model (exit ${result})\n"
            "command: ${command}\n${output}"
        )
    endif()
else()
    if(result EQUAL 0)
        message(FATAL_ERROR
            "ilic unexpectedly accepted an invalid model\n"
            "command: ${command}\n${output}"
        )
    endif()
endif()

if(DEFINED OUTPUT_REGEX AND NOT OUTPUT_REGEX STREQUAL "")
    if(NOT output MATCHES "${OUTPUT_REGEX}")
        message(FATAL_ERROR
            "ilic output does not match '${OUTPUT_REGEX}'\n${output}"
        )
    endif()
endif()
