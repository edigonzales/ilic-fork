if(NOT DEFINED ILIC_TEST_HARNESS_CANARY OR ILIC_TEST_HARNESS_CANARY STREQUAL "")
   message(FATAL_ERROR "ILIC_TEST_HARNESS_CANARY must name the canary executable")
endif()

execute_process(
   COMMAND "${ILIC_TEST_HARNESS_CANARY}" --pass
   RESULT_VARIABLE pass_result
   OUTPUT_VARIABLE pass_output
   ERROR_VARIABLE pass_error
)
if(NOT "${pass_result}" STREQUAL "0")
   message(FATAL_ERROR
      "Test harness canary --pass failed with exit code ${pass_result}.\n"
      "stdout:\n${pass_output}\n"
      "stderr:\n${pass_error}")
endif()

execute_process(
   COMMAND "${ILIC_TEST_HARNESS_CANARY}" --fail
   RESULT_VARIABLE fail_result
   OUTPUT_VARIABLE fail_output
   ERROR_VARIABLE fail_error
)
if("${fail_result}" STREQUAL "0")
   message(FATAL_ERROR
      "Test harness canary --fail unexpectedly succeeded.\n"
      "stdout:\n${fail_output}\n"
      "stderr:\n${fail_error}")
endif()

set(failure_output "${fail_output}\n${fail_error}")
string(FIND "${failure_output}" "ILIC_TEST_FAILURE" marker_position)
if(marker_position EQUAL -1)
   message(FATAL_ERROR
      "Test harness canary --fail did not emit ILIC_TEST_FAILURE.\n"
      "output:\n${failure_output}")
endif()
string(FIND "${failure_output}" "intentional canary failure" message_position)
if(message_position EQUAL -1)
   message(FATAL_ERROR
      "Test harness canary --fail did not emit the intentional failure message.\n"
      "output:\n${failure_output}")
endif()
