cmake_minimum_required(VERSION 3.20)

foreach(required_variable IN ITEMS ANTLR_ARCHIVE SOURCE_ROOT WORK_ROOT)
    if(NOT DEFINED ${required_variable})
        message(FATAL_ERROR "Missing required variable: ${required_variable}")
    endif()
endforeach()

find_program(JAVA_EXECUTABLE NAMES java)
if(NOT JAVA_EXECUTABLE)
    message(FATAL_ERROR
        "Parser regeneration requires Java, but no 'java' executable was found."
    )
endif()

if(NOT EXISTS "${ANTLR_ARCHIVE}")
    message(FATAL_ERROR "ANTLR archive not found: ${ANTLR_ARCHIVE}")
endif()

set(GRAMMAR_ROOT "${SOURCE_ROOT}/source/input/parser/grammar")
set(GENERATED_ROOT "${SOURCE_ROOT}/source/input/parser/generated")
set(ANTLR_ROOT "${WORK_ROOT}/antlr")
set(REGENERATED_ROOT "${WORK_ROOT}/generated")
set(ANTLR_JAR "${ANTLR_ROOT}/java/antlr-4.7.1-complete.jar")

file(REMOVE_RECURSE "${WORK_ROOT}")
file(MAKE_DIRECTORY "${WORK_ROOT}")

file(ARCHIVE_EXTRACT
    INPUT "${ANTLR_ARCHIVE}"
    DESTINATION "${ANTLR_ROOT}"
    PATTERNS "java/antlr-4.7.1-complete.jar"
)

if(NOT EXISTS "${ANTLR_JAR}")
    message(FATAL_ERROR "ANTLR 4.7.1 JAR was not extracted from ${ANTLR_ARCHIVE}")
endif()

file(MAKE_DIRECTORY "${REGENERATED_ROOT}")

set(GRAMMAR_FILES
    IliFileLexer.l4
    IliFileParser.g4
    Ili1Lexer.l4
    Ili1Parser.g4
    Ili2Lexer.l4
    Ili2Parser.g4
)
foreach(grammar IN LISTS GRAMMAR_FILES)
    configure_file(
        "${GRAMMAR_ROOT}/${grammar}"
        "${REGENERATED_ROOT}/${grammar}"
        COPYONLY
    )
endforeach()

function(run_antlr)
    execute_process(
        COMMAND "${JAVA_EXECUTABLE}"
            -cp "${ANTLR_JAR}"
            org.antlr.v4.Tool
            -Dlanguage=Cpp
            ${ARGN}
        WORKING_DIRECTORY "${REGENERATED_ROOT}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )

    if(NOT result EQUAL 0)
        message(FATAL_ERROR
            "ANTLR 4.7.1 failed with exit code ${result}\n"
            "stdout:\n${output}\n"
            "stderr:\n${error_output}"
        )
    endif()
endfunction()

run_antlr(-package lexer IliFileLexer.l4)
run_antlr(-package parser -visitor -no-listener IliFileParser.g4)
run_antlr(-package lexer Ili1Lexer.l4)
run_antlr(-package parser -visitor -no-listener Ili1Parser.g4)
run_antlr(-package lexer Ili2Lexer.l4)
run_antlr(-package parser -visitor -no-listener Ili2Parser.g4)

file(REMOVE
    "${REGENERATED_ROOT}/IliFileLexer.l4"
    "${REGENERATED_ROOT}/IliFileParser.g4"
    "${REGENERATED_ROOT}/Ili1Lexer.l4"
    "${REGENERATED_ROOT}/Ili1Parser.g4"
    "${REGENERATED_ROOT}/Ili2Lexer.l4"
    "${REGENERATED_ROOT}/Ili2Parser.g4"
)

set(GENERATED_PATTERNS "*.cpp" "*.h" "*.interp" "*.tokens")
set(EXPECTED_GLOB_PATTERNS)
set(REGENERATED_GLOB_PATTERNS)
foreach(pattern IN LISTS GENERATED_PATTERNS)
    list(APPEND EXPECTED_GLOB_PATTERNS "${GENERATED_ROOT}/${pattern}")
    list(APPEND REGENERATED_GLOB_PATTERNS "${REGENERATED_ROOT}/${pattern}")
endforeach()
file(GLOB EXPECTED_FILES RELATIVE "${GENERATED_ROOT}" ${EXPECTED_GLOB_PATTERNS})
file(GLOB REGENERATED_FILES RELATIVE "${REGENERATED_ROOT}" ${REGENERATED_GLOB_PATTERNS})
list(SORT EXPECTED_FILES)
list(SORT REGENERATED_FILES)

set(MISSING_FILES ${EXPECTED_FILES})
list(REMOVE_ITEM MISSING_FILES ${REGENERATED_FILES})
set(EXTRA_FILES ${REGENERATED_FILES})
list(REMOVE_ITEM EXTRA_FILES ${EXPECTED_FILES})

set(DIFFERENCES)
foreach(relative_file IN LISTS EXPECTED_FILES)
    if(EXISTS "${REGENERATED_ROOT}/${relative_file}")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E compare_files
                "${GENERATED_ROOT}/${relative_file}"
                "${REGENERATED_ROOT}/${relative_file}"
            RESULT_VARIABLE compare_result
        )
        if(NOT compare_result EQUAL 0)
            list(APPEND DIFFERENCES ${relative_file})
        endif()
    endif()
endforeach()

if(MISSING_FILES OR EXTRA_FILES OR DIFFERENCES)
    message(FATAL_ERROR
        "ANTLR 4.7.1 parser regeneration differs from the checked-in files.\n"
        "Missing: ${MISSING_FILES}\n"
        "Extra: ${EXTRA_FILES}\n"
        "Different: ${DIFFERENCES}\n"
        "The regenerated files are available under ${REGENERATED_ROOT}."
    )
endif()

list(LENGTH EXPECTED_FILES GENERATED_FILE_COUNT)
message(STATUS
    "ANTLR 4.7.1 parser regeneration matches ${GENERATED_FILE_COUNT} checked-in files."
)
