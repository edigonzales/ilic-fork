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
    source/metamodel/SemanticChecker.cpp
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
    add_ilic_regression_test(ilic_regression_expression_relations_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/expression_relations_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_expression_relations_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/expression_relations_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_expression_boolean_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/expression_boolean_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_expression_boolean_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/expression_boolean_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_existence_types_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/existence_types_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_existence_types_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/existence_types_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_expression_arithmetic_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/expression_arithmetic_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_expression_arithmetic_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/expression_arithmetic_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_domain_constraints_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/domain_constraints_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_domain_constraints_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/domain_constraints_invalid.ili"
    )
    add_test(
        NAME ilic_regression_domain_constraints_roundtrip
        COMMAND "${CMAKE_COMMAND}"
            "-DILIC=$<TARGET_FILE:ilic>"
            "-DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/domain_constraints_valid.ili"
            "-DOUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/domain-constraints-roundtrip.ili"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_ilic_domain_roundtrip.cmake"
    )
    add_ilic_regression_test(ilic_regression_association_roles_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/association_roles_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_association_cardinality_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/association_cardinality_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_association_strength_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/association_strength_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_association_target_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/association_target_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_association_visibility_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/association_visibility_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_association_duplicate_role_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/association_duplicate_role_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_attribute_extension_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/attribute_extension_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_attribute_cardinality_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/attribute_cardinality_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_attribute_type_extension_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/attribute_type_extension_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_enum_extension_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/enum_extension_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_enum_extension_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/enum_extension_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_line_overlap_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/line_overlap_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_line_overlap_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/line_overlap_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_class_specialization_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/class_specialization_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_class_same_name_extends_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/class_same_name_extends_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_class_extended_with_local_specialization_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/class_extended_with_local_specialization_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_class_extended_with_inherited_specialization_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/class_extended_with_inherited_specialization_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_abstract_view_in_concrete_topic_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/abstract_view_in_concrete_topic_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_inherited_abstract_class_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/inherited_abstract_class_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_inherited_abstract_class_concretized_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/inherited_abstract_class_concretized_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_model_end_name_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/model_end_name_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_topic_end_name_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/topic_end_name_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_class_end_name_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/class_end_name_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_package_namespace_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/package_namespace_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_runtime_parameter_namespace_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/runtime_parameter_namespace_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_view_attribute_namespace_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/view_attribute_namespace_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_declaration_namespaces_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/declaration_namespaces_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_model_level_reference_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/model_level_reference_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_existence_topic_dependency_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/existence_topic_dependency_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_view_topic_dependency_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/view_topic_dependency_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_graphic_topic_dependency_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/graphic_topic_dependency_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_topic_dependencies_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/topic_dependencies_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_model_import_unit_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/model_import_unit_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_model_import_metadata_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/model_import_metadata_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_model_imports_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/model_imports_valid.ili"
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
