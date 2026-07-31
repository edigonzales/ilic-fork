function(ilic_configure_sanitizers)
   if(ILIC_ENABLE_SANITIZERS AND ILIC_ENABLE_THREAD_SANITIZER)
      message(FATAL_ERROR
         "ILIC_ENABLE_SANITIZERS and ILIC_ENABLE_THREAD_SANITIZER are mutually exclusive")
   endif()

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

function(ilic_configure_thread_sanitizer)
   if(NOT ILIC_ENABLE_THREAD_SANITIZER)
      return()
   endif()

   if(EMSCRIPTEN)
      message(FATAL_ERROR
         "ILIC_ENABLE_THREAD_SANITIZER is not supported for Emscripten/WASM builds")
   endif()

   if(ILIC_ENABLE_SANITIZERS)
      message(FATAL_ERROR
         "ILIC_ENABLE_SANITIZERS and ILIC_ENABLE_THREAD_SANITIZER are mutually exclusive")
   endif()

   if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$" OR
      NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
      message(FATAL_ERROR
         "ILIC_ENABLE_THREAD_SANITIZER requires GCC or Clang-compatible compilers")
   endif()

   add_compile_options(
      -fsanitize=thread
      -fno-omit-frame-pointer
      -fno-sanitize-recover=all
   )
   add_link_options(
      -fsanitize=thread
      -fno-omit-frame-pointer
      -fno-sanitize-recover=all
   )
endfunction()
