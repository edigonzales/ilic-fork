include_guard(GLOBAL)

set(_ilic_install_default OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR AND
   NOT ILIC_STATIC_DISTRIBUTION)
    set(_ilic_install_default ON)
endif()
option(ILIC_ENABLE_INSTALL
    "Generate install rules and CMake package metadata"
    ${_ilic_install_default}
)
unset(_ilic_install_default)

function(ilic_configure_packaging)
    if(NOT ILIC_ENABLE_INSTALL OR EMSCRIPTEN)
        return()
    endif()

    foreach(required_target
        antlr4-runtime
        ilic-core
        ilic-json
        ilic-capi
    )
        if(NOT TARGET ${required_target})
            message(FATAL_ERROR
                "ILIC_ENABLE_INSTALL requires target ${required_target}")
        endif()
    endforeach()

    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Keep source-tree consumers unchanged while making exported targets
    # relocatable. The bundled ANTLR and JSON targets are implementation
    # details: installed consumers only need them for static-library linking,
    # not for their headers.
    set_property(TARGET antlr4-runtime PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib/antlr4/include>")
    set_property(TARGET antlr4-runtime PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib/antlr4/include>")
    set_property(TARGET ilic-core PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>;$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    set_property(TARGET ilic-json PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/abi>")
    set_property(TARGET ilic-capi PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>;$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

    # Preserve the source-tree target names and aliases, but publish a stable
    # package namespace for find_package(ilic CONFIG).
    set_target_properties(antlr4-runtime PROPERTIES EXPORT_NAME detail-antlr4-runtime)
    set_target_properties(ilic-json PROPERTIES EXPORT_NAME detail-json)
    set_target_properties(ilic-core PROPERTIES EXPORT_NAME core)
    set_target_properties(ilic-capi PROPERTIES EXPORT_NAME capi)

    install(TARGETS
        antlr4-runtime
        ilic-core
        ilic-json
        ilic-capi
        EXPORT ilicTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )

    if(TARGET ilic)
        install(TARGETS ilic RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    endif()
    if(TARGET ilic-format)
        install(TARGETS ilic-format RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    endif()

    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/ilic"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.h"
    )

    set(_ilic_package_dir "${CMAKE_INSTALL_LIBDIR}/cmake/ilic")
    file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/cmake")
    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/ilicConfig.cmake.in"
        "${PROJECT_BINARY_DIR}/cmake/ilicConfig.cmake"
        INSTALL_DESTINATION "${_ilic_package_dir}"
    )
    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/cmake/ilicConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion
    )

    install(EXPORT ilicTargets
        FILE ilicTargets.cmake
        NAMESPACE ilic::
        DESTINATION "${_ilic_package_dir}"
    )
    install(FILES
        "${PROJECT_BINARY_DIR}/cmake/ilicConfig.cmake"
        "${PROJECT_BINARY_DIR}/cmake/ilicConfigVersion.cmake"
        DESTINATION "${_ilic_package_dir}"
    )

    if(BUILD_TESTING AND ILIC_BUILD_TESTS)
        add_test(
            NAME ilic_installed_package
            COMMAND "${CMAKE_COMMAND}"
                "-DILIC_BUILD_DIR=${PROJECT_BINARY_DIR}"
                "-DILIC_CONSUMER_SOURCE_DIR=${PROJECT_SOURCE_DIR}/test/consumer/installed-package"
                "-DILIC_TEST_ROOT=${PROJECT_BINARY_DIR}/installed-package-test"
                "-DILIC_CONSUMER_EXE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}"
                "-DILIC_GENERATOR=${CMAKE_GENERATOR}"
                "-DILIC_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
                "-DILIC_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
                "-DILIC_BUILD_CONFIG=$<CONFIG>"
                -P "${PROJECT_SOURCE_DIR}/cmake/run_installed_package_test.cmake"
        )
        set_tests_properties(ilic_installed_package PROPERTIES LABELS "packaging")
    endif()
endfunction()

# The project defines its libraries after the distribution helper is included.
# Defer install/export setup until all targets exist. Embedded add_subdirectory,
# FetchContent, and static-distribution users keep this disabled by default.
if(ILIC_ENABLE_INSTALL AND NOT EMSCRIPTEN)
    cmake_language(DEFER CALL ilic_configure_packaging)
endif()
