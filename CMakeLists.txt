cmake_minimum_required(VERSION 3.20)

project(ilic VERSION 0.9.9 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

file(GLOB_RECURSE ANTLR_RUNTIME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/antlr4/include/*.cpp"
)

add_library(antlr4-runtime STATIC
    ${ANTLR_RUNTIME_SOURCES}
)

target_include_directories(antlr4-runtime
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/antlr4/include"
)

add_executable(ilic
    source/input/ili1/Ili1Input.cpp
    source/input/ili1/Ili1Input_package.cpp
    source/input/ili1/Ili1Input_table.cpp
    source/input/ili1/Ili1Input_type.cpp

    source/input/ili2/Ili2Input.cpp
    source/input/ili2/Ili2Input_assoc.cpp
    source/input/ili2/Ili2Input_class.cpp
    source/input/ili2/Ili2Input_constraint.cpp
    source/input/ili2/Ili2Input_expression.cpp
    source/input/ili2/Ili2Input_function.cpp
    source/input/ili2/Ili2Input_graphic.cpp
    source/input/ili2/Ili2Input_helper.cpp
    source/input/ili2/Ili2Input_package.cpp
    source/input/ili2/Ili2Input_path.cpp
    source/input/ili2/Ili2Input_type.cpp
    source/input/ili2/Ili2Input_unit.cpp
    source/input/ili2/Ili2Input_view.cpp
    source/input/ili2/InterlisModel.cpp

    source/input/parser/IliParserErrorListener.cpp

    source/main/ilic.cpp

    source/metamodel/MetaModel.cpp
    source/metamodel/MetaModelInput.cpp
    source/metamodel/MetaModelOutput.cpp
    source/metamodel/MetaModelTreeVisitor.cpp

    source/output/GmlOutput.cpp
    source/output/Ili1Output.cpp
    source/output/Ili2Output.cpp
    source/output/ImdOutput.cpp
    source/output/XsdOutput.cpp

    source/util/Converter.cpp
    source/util/IliFile.cpp
    source/util/Logger.cpp
    source/util/os.cpp
    source/util/StringMap.cpp
    source/util/StringUtil.cpp
    source/util/TextWriter.cpp
    source/util/XMLWriter.cpp
    source/util/XTFWriter.cpp
)

target_link_libraries(ilic PRIVATE antlr4-runtime)

if(BUILD_TESTING)
    add_test(
        NAME ilic_version
        COMMAND ilic -v
    )
    set_tests_properties(ilic_version PROPERTIES
        PASS_REGULAR_EXPRESSION "platform=macos64"
    )

    add_test(
        NAME ilic_validate_dataset_idx16
        COMMAND ilic
            -silent
            "${CMAKE_CURRENT_SOURCE_DIR}/test/ili23/core/DatasetIdx16.ili"
    )

    add_test(
        NAME ilic_import_search
        COMMAND ilic
            -silent
            -ilidirs
            "${CMAKE_CURRENT_SOURCE_DIR}/test/ili23/Roads"
            "${CMAKE_CURRENT_SOURCE_DIR}/test/ili23/Roads/RoadsExdm2ien.ili"
    )

    set(ILIC_SMOKE_XSD_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ilic-smoke.xsd")
    add_test(
        NAME ilic_xsd_output
        COMMAND "${CMAKE_COMMAND}"
            "-DILIC=$<TARGET_FILE:ilic>"
            "-DOUTPUT_FILE=${ILIC_SMOKE_XSD_OUTPUT}"
            "-DILIDIR=${CMAKE_CURRENT_SOURCE_DIR}/test/ili23/Roads"
            "-DMODEL=${CMAKE_CURRENT_SOURCE_DIR}/test/ili23/Roads/RoadsExdm2ien.ili"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_ilic_xsd_smoke.cmake"
    )
endif()

add_custom_target(check-parser-regeneration
    COMMAND "${CMAKE_COMMAND}"
        "-DANTLR_ARCHIVE=${CMAKE_CURRENT_SOURCE_DIR}/lib/antlr4/bin.zip"
        "-DSOURCE_ROOT=${CMAKE_CURRENT_SOURCE_DIR}"
        "-DWORK_ROOT=${CMAKE_CURRENT_BINARY_DIR}/parser-regeneration"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/check_parser_regeneration.cmake"
    USES_TERMINAL
    COMMENT "Checking generated parsers against ANTLR 4.7.1"
)
