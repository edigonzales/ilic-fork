vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO edigonzales/ilic-fork
    REF bb284a67d62d10e62ada2491ed9db0f6ef446535
    SHA512 9b5fe6e570f98075ee4a6eece9d6d20c420a633c2b6ab7eab6469f9e14627be79ec3ecac3b8fe7c65447e424759b323ff5ff00a977b1b24bc967feeb9503ce65
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

# The source repository currently has no project-level LICENSE file. Do not
# invent a license in the package recipe. This temporary policy is acceptable
# for the in-repository overlay validation, but must be removed before moving
# ilic into a public/custom registry intended for general redistribution.
set(VCPKG_POLICY_SKIP_COPYRIGHT_CHECK enabled)
