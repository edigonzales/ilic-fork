vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO edigonzales/ilic-fork
    REF d335fcf8f7fbbaf82586a597dfa7944d5e5e8938
    SHA512 0
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBUILD_TESTING=OFF
        -DILIC_BUILD_TESTS=OFF
        -DILIC_BUILD_CLI=OFF
        -DILIC_ENABLE_NATIVE_REPOSITORY=OFF
        -DILIC_STATIC_DISTRIBUTION=OFF
        -DILIC_ENABLE_INSTALL=ON
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(
    PACKAGE_NAME ilic
    CONFIG_PATH lib/cmake/ilic
)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/copyright"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
