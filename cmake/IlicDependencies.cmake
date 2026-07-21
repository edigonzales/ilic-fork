include(FetchContent)

function(ilic_configure_repository_dependencies curl_target xml_target)
    # pugixml is intentionally compiled from an immutable source archive so
    # repository XML never adds a runtime library dependency.
    FetchContent_Declare(ilic_pugixml
        URL https://github.com/zeux/pugixml/archive/refs/tags/v1.14.tar.gz
        URL_HASH SHA256=610f98375424b5614754a6f34a491adbddaaec074e9044577d965160ec103d2e
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        SOURCE_SUBDIR ilic-no-cmake-project
    )
    FetchContent_MakeAvailable(ilic_pugixml)
    if(NOT TARGET ilic-pugixml)
        add_library(ilic-pugixml STATIC
            "${ilic_pugixml_SOURCE_DIR}/src/pugixml.cpp"
        )
        target_include_directories(ilic-pugixml PUBLIC
            "${ilic_pugixml_SOURCE_DIR}/src"
        )
        add_library(pugixml::pugixml ALIAS ilic-pugixml)
    endif()

    if(ILIC_STATIC_DISTRIBUTION)
        # curl 8.10.1 is built as a small HTTP(S)-only static library. Platform
        # TLS backends avoid third-party TLS DLLs/dylibs on macOS and Windows.
        set(_ilic_saved_build_shared_libs "${BUILD_SHARED_LIBS}")
        set(_ilic_saved_build_testing "${BUILD_TESTING}")
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
        set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
        set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
        set(BUILD_LIBCURL_DOCS OFF CACHE BOOL "" FORCE)
        set(BUILD_MISC_DOCS OFF CACHE BOOL "" FORCE)
        set(CURL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
        set(CURL_ENABLE_EXPORT_TARGET OFF CACHE BOOL "" FORCE)
        set(CURL_USE_PKGCONFIG OFF CACHE BOOL "" FORCE)
        set(HTTP_ONLY ON CACHE BOOL "" FORCE)
        set(CURL_ENABLE_SSL ON CACHE BOOL "" FORCE)
        set(CURL_BROTLI OFF CACHE BOOL "" FORCE)
        set(CURL_ZSTD OFF CACHE BOOL "" FORCE)
        set(CURL_ZLIB OFF CACHE BOOL "" FORCE)
        set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE CACHE BOOL "" FORCE)
        set(USE_NGHTTP2 OFF CACHE BOOL "" FORCE)
        set(USE_NGTCP2 OFF CACHE BOOL "" FORCE)
        set(USE_QUICHE OFF CACHE BOOL "" FORCE)
        set(USE_MSH3 OFF CACHE BOOL "" FORCE)
        set(USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
        set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
        set(CURL_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
        set(CURL_USE_LIBSSH OFF CACHE BOOL "" FORCE)
        set(CURL_USE_GSSAPI OFF CACHE BOOL "" FORCE)
        set(CURL_USE_GSASL OFF CACHE BOOL "" FORCE)
        set(ENABLE_ARES OFF CACHE BOOL "" FORCE)
        set(ENABLE_CURL_MANUAL OFF CACHE BOOL "" FORCE)
        set(ENABLE_WEBSOCKETS OFF CACHE BOOL "" FORCE)
        if(APPLE)
            set(CURL_USE_SECTRANSP ON CACHE BOOL "" FORCE)
            set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
        elseif(WIN32)
            set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
            set(CURL_WINDOWS_SSPI ON CACHE BOOL "" FORCE)
            set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
            set(CURL_STATIC_CRT ON CACHE BOOL "" FORCE)
        else()
            set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
            set(OPENSSL_USE_STATIC_LIBS TRUE CACHE BOOL "" FORCE)
        endif()
        FetchContent_Declare(ilic_curl
            URL https://curl.se/download/curl-8.10.1.tar.xz
            URL_HASH SHA256=73a4b0e99596a09fa5924a4fb7e4b995a85fda0d18a2c02ab9cf134bebce04ee
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        FetchContent_MakeAvailable(ilic_curl)
        set(BUILD_SHARED_LIBS "${_ilic_saved_build_shared_libs}" CACHE BOOL "" FORCE)
        set(BUILD_TESTING "${_ilic_saved_build_testing}" CACHE BOOL "" FORCE)
    else()
        find_package(CURL REQUIRED)
    endif()

    set(${curl_target} CURL::libcurl PARENT_SCOPE)
    set(${xml_target} pugixml::pugixml PARENT_SCOPE)
endfunction()
