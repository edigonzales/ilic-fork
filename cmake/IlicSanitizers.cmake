function(ilic_configure_sanitizers)
   if(NOT ILIC_ENABLE_SANITIZERS)
      return()
   endif()

   if(EMSCRIPTEN)
      message(FATAL_ERROR
         "ILIC_ENABLE_SANITIZERS is not supported for Emscripten/WASM builds")
   endif()

   if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$" OR
      NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
      message(FATAL_ERROR
         "ILIC_ENABLE_SANITIZERS currently requires GCC or Clang-compatible C and C++ compilers")
   endif()

   add_compile_options(
      -fsanitize=address,undefined
      -fno-omit-frame-pointer
      -fno-sanitize-recover=all
   )
   add_link_options(
      -fsanitize=address,undefined
      -fno-sanitize-recover=all
   )
endfunction()
