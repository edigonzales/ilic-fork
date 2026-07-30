if(NOT DEFINED ILIC_SOURCE_DIR OR ILIC_SOURCE_DIR STREQUAL "")
   message(FATAL_ERROR "ILIC_SOURCE_DIR must name the repository root")
endif()

set(test_root "${ILIC_SOURCE_DIR}/test")
file(GLOB_RECURSE test_sources
   "${test_root}/*.cpp"
   "${test_root}/*.cc"
   "${test_root}/*.cxx"
   "${test_root}/*.h"
   "${test_root}/*.hpp"
)

set(violations)
foreach(source_file IN LISTS test_sources)
   file(STRINGS "${source_file}" source_lines)
   set(in_block_comment FALSE)
   set(line_number 0)
   foreach(source_line IN LISTS source_lines)
      math(EXPR line_number "${line_number} + 1")
      set(code_line "${source_line}")

      # Remove block and line comments before looking for assertion syntax.
      # The loop also handles a block comment that spans multiple lines.
      while(TRUE)
         if(in_block_comment)
            string(FIND "${code_line}" "*/" block_end)
            if(block_end EQUAL -1)
               set(code_line "")
               break()
            endif()
            math(EXPR after_comment_start "${block_end} + 2")
            string(SUBSTRING "${code_line}" "${after_comment_start}" -1 code_line)
            set(in_block_comment FALSE)
         endif()

         string(FIND "${code_line}" "/*" block_start)
         if(block_start EQUAL -1)
            break()
         endif()
         string(SUBSTRING "${code_line}" 0 "${block_start}" before_comment)
         math(EXPR after_comment_start "${block_start} + 2")
         string(SUBSTRING "${code_line}" "${after_comment_start}" -1 after_comment)
         string(FIND "${after_comment}" "*/" block_end)
         if(block_end EQUAL -1)
            set(code_line "${before_comment}")
            set(in_block_comment TRUE)
            break()
         endif()
         math(EXPR after_comment_start "${block_end} + 2")
         string(SUBSTRING "${after_comment}" "${after_comment_start}" -1 after_comment)
         set(code_line "${before_comment}${after_comment}")
      endwhile()

      string(REGEX REPLACE "//.*$" "" code_line "${code_line}")
      if(code_line MATCHES "#[ \t]*include[ \t]*[<\"]cassert[>\"]")
         list(APPEND violations
            "  ${source_file}:${line_number}: includes <cassert> or \"cassert\"")
      endif()
      if(code_line MATCHES "(^|[^A-Za-z0-9_])assert[ \t]*\\(")
         list(APPEND violations
            "  ${source_file}:${line_number}: calls the standard assert() name")
      endif()
   endforeach()
endforeach()

if(violations)
   string(JOIN "\n" violation_text ${violations})
   message(FATAL_ERROR
      "Native tests must use ilic/test/TestHarness.h instead of <cassert>/assert().\n"
      "Violations:\n${violation_text}")
endif()
