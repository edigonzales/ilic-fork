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
    source/metamodel/TranslationChecker.cpp

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
    function(add_ilic_regression_test name expected_success)
        set(options)
        set(one_value_args OUTPUT_REGEX ILIDIRS)
        set(multi_value_args INPUTS)
        cmake_parse_arguments(TEST "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

        add_test(
            NAME "${name}"
            COMMAND "${CMAKE_COMMAND}"
                "-DILIC=$<TARGET_FILE:ilic>"
                "-DEXPECT_SUCCESS=${expected_success}"
                "-DINPUTS=${TEST_INPUTS}"
                "-DILIDIRS=${TEST_ILIDIRS}"
                "-DOUTPUT_REGEX=${TEST_OUTPUT_REGEX}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_ilic_regression.cmake"
        )
    endfunction()

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

    add_ilic_regression_test(ilic_regression_reference_to_anyclass TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/reference_to_anyclass.ili"
    )
    add_ilic_regression_test(ilic_regression_unknown_domain FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/unknown_domain.ili"
    )
    add_ilic_regression_test(ilic_regression_union_view TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/union_view.ili"
    )
    add_ilic_regression_test(ilic_regression_parent_path TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/parent_path.ili"
    )
    add_ilic_regression_test(ilic_regression_date_time_types TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/date_time_types.ili"
    )
    add_ilic_regression_test(ilic_regression_list_of_text TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/list_of_text.ili"
    )
    add_ilic_regression_test(ilic_regression_roads_graphic TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/ili23/Roads/RoadsExgm2ien.ili"
        ILIDIRS "${CMAKE_CURRENT_SOURCE_DIR}/test/ili23/Roads"
    )
    add_ilic_regression_test(ilic_regression_lexer_scaling_name TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/lexer_scaling_name.ili"
    )
    add_ilic_regression_test(ilic_regression_lexer_nested_comment TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/lexer_nested_comment.ili"
    )
    add_ilic_regression_test(ilic_regression_lexer_unterminated_comment FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/lexer_unterminated_comment.ili"
    )
    add_ilic_regression_test(ilic_regression_lexer_malformed_scaling FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/lexer_malformed_scaling.ili"
    )
    add_ilic_regression_test(ilic_regression_attribute_path_non_class FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/attribute_path_non_class.ili"
    )
    add_ilic_regression_test(ilic_regression_attribute_path_class TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/attribute_path_class.ili"
    )
    add_ilic_regression_test(ilic_regression_transient_without_derivation FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/transient_without_derivation.ili"
    )
    add_ilic_regression_test(ilic_regression_transient_with_derivation TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/transient_with_derivation.ili"
    )
    add_ilic_regression_test(ilic_regression_transient_abstract_without_derivation TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/transient_abstract_without_derivation.ili"
    )
    add_ilic_regression_test(ilic_regression_path_parent_in_area FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/path_parent_in_area.ili"
    )
    add_ilic_regression_test(ilic_regression_path_thisarea_in_normal FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/path_thisarea_in_normal.ili"
    )
    add_ilic_regression_test(ilic_regression_path_thatarea_in_normal FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/path_thatarea_in_normal.ili"
    )
    add_ilic_regression_test(ilic_regression_path_contextual_extension TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/path_contextual_extension.ili"
    )

    add_ilic_regression_test(ilic_regression_translation_complete TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/translation_complete.ili"
    )
    add_ilic_regression_test(ilic_regression_translation_mismatch FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/translation_mismatch.ili"
        OUTPUT_REGEX "translation mismatch"
    )
    add_ilic_regression_test(ilic_regression_translation_dependency_loading TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/translation_loading/TranslatedModel.ili"
        ILIDIRS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/translation_loading"
    )
    add_ilic_regression_test(ilic_regression_translation_missing_base FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/translation_missing_base.ili"
    )
    add_ilic_regression_test(ilic_regression_translation_self FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/translation_self.ili"
    )
    add_ilic_regression_test(ilic_regression_translation_cycle FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/translation_cycle.ili"
    )
    add_test(
        NAME ilic_regression_translation_roundtrip
        COMMAND "${CMAKE_COMMAND}"
            "-DILIC=$<TARGET_FILE:ilic>"
            "-DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/translation_complete.ili"
            "-DOUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/translation-roundtrip.ili"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_ilic_translation_roundtrip.cmake"
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
