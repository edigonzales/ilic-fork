if(ILIC_STATIC_DISTRIBUTION)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY
            "MultiThreaded$<$<CONFIG:Debug>:Debug>"
            CACHE STRING "Static MSVC runtime for distributable binaries" FORCE)
    endif()
endif()

function(ilic_enable_static_distribution target)
    if(NOT ILIC_STATIC_DISTRIBUTION)
        return()
    endif()
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_options(${target} PRIVATE -static)
    endif()
endfunction()

# Distribution setup is included before the ilic targets are declared. The
# packaging module uses a deferred callback and is top-level-only by default,
# so embedded add_subdirectory/FetchContent consumers keep their current
# target and dependency behavior.
include("${CMAKE_CURRENT_LIST_DIR}/IlicPackaging.cmake")
