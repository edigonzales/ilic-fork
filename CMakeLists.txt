cmake_minimum_required(VERSION 3.20)

project(ilic VERSION 0.9.9 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

option(ILIC_ENABLE_NATIVE_REPOSITORY "Build native HTTP and filesystem repository support" ON)

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

set(ILIC_CORE_SOURCES
    source/core/Compiler.cpp
    source/core/Formatter.cpp
    source/core/SourceManager.cpp
    source/core/Syntax.cpp
    source/core/Semantic.cpp

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

add_library(ilic-core STATIC ${ILIC_CORE_SOURCES})
add_library(ilic::core ALIAS ilic-core)
# The IlisMeta schema uses role names such as `Type`, `Class`, and
# `MetaElement` that intentionally match their C++ type names. GCC 14 and
# newer diagnose these legacy declarations as errors unless this narrowly
# scoped compatibility diagnostic is disabled. Keep the schema-derived API
# stable instead of renaming its public members.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
   CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
    target_compile_options(ilic-core PUBLIC
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-changes-meaning>
    )
endif()
target_include_directories(ilic-core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(ilic-core PUBLIC antlr4-runtime)

add_library(ilic-capi STATIC
    source/abi/Capi.cpp
    source/abi/Json.cpp
)
add_library(ilic::capi ALIAS ilic-capi)
target_include_directories(ilic-capi
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(ilic-capi PUBLIC ilic-core)

add_executable(ilic source/main/ilic.cpp)
target_link_libraries(ilic PRIVATE ilic-capi)

add_executable(ilic-format source/main/ilic_format.cpp)
target_link_libraries(ilic-format PRIVATE ilic-core)

if(EMSCRIPTEN)
    add_executable(ilic-wasm
        source/abi/Capi.cpp
        source/abi/Json.cpp
    )
    target_include_directories(ilic-wasm PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_link_libraries(ilic-wasm PRIVATE ilic-core)
    target_link_options(ilic-wasm PRIVATE
        "--no-entry"
        "-sMODULARIZE=1"
        "-sEXPORT_ES6=1"
        "-sENVIRONMENT=web,worker,node"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sWASM_BIGINT=1"
        "-sASSERTIONS=1"
        "SHELL:-sEXPORTED_FUNCTIONS=['_ilic_abi_version','_ilic_version','_ilic_alloc','_ilic_free','_ilic_session_create','_ilic_session_destroy','_ilic_session_put_source','_ilic_session_remove_source','_ilic_parse','_ilic_analyze','_ilic_compile','_ilic_format','_ilic_result_json','_ilic_result_destroy']"
    )
    set_target_properties(ilic-wasm PROPERTIES OUTPUT_NAME "ilic" SUFFIX ".mjs")
endif()

if(ILIC_ENABLE_NATIVE_REPOSITORY AND NOT EMSCRIPTEN)
    find_package(CURL REQUIRED)
    find_package(LibXml2 REQUIRED)
    add_library(ilic-repository STATIC
        source/repository/Md5.cpp
        source/repository/Repository.cpp
    )
    add_library(ilic::repository ALIAS ilic-repository)
    target_include_directories(ilic-repository
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
    target_link_libraries(ilic-repository
        PUBLIC ilic-core CURL::libcurl LibXml2::LibXml2
    )
    target_link_libraries(ilic PRIVATE ilic-repository)
    target_compile_definitions(ilic PRIVATE ILIC_NATIVE_REPOSITORY=1)
endif()

if(BUILD_TESTING)
    add_executable(ilic_docs_cpp_example docs/examples/cpp-session.cpp)
    target_link_libraries(ilic_docs_cpp_example PRIVATE ilic-core)
    add_test(NAME ilic_docs_cpp_example COMMAND ilic_docs_cpp_example)

    add_executable(ilic_docs_c_example docs/examples/c-api.c)
    set_target_properties(ilic_docs_c_example PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(ilic_docs_c_example PRIVATE ilic-capi)
    add_test(NAME ilic_docs_c_example COMMAND ilic_docs_c_example)

    if(UNIX)
        add_test(NAME ilic_docs_cli_examples
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/docs/examples/cli.sh"
                "$<TARGET_FILE:ilic>"
                "$<TARGET_FILE:ilic-format>"
                "${CMAKE_CURRENT_SOURCE_DIR}"
                "${CMAKE_CURRENT_BINARY_DIR}/docs-example-output")
    endif()

    find_program(ILIC_NODE_EXECUTABLE node)
    if(ILIC_NODE_EXECUTABLE)
        add_test(NAME ilic_docs_links
            COMMAND "${ILIC_NODE_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/test/docs/DocumentationTest.mjs"
                "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    add_executable(ilic_capi_test test/abi/CapiTest.cpp)
    target_link_libraries(ilic_capi_test PRIVATE ilic-capi)
    add_test(NAME ilic_capi COMMAND ilic_capi_test)
    add_executable(ilic_core_session_test test/core/CompilerSessionTest.cpp)
    target_link_libraries(ilic_core_session_test PRIVATE ilic-core)
    add_test(NAME ilic_core_session COMMAND ilic_core_session_test)
    add_executable(ilic_syntax_snapshot_test test/core/SyntaxSnapshotTest.cpp)
    target_link_libraries(ilic_syntax_snapshot_test PRIVATE ilic-core)
    add_test(NAME ilic_syntax_snapshot COMMAND ilic_syntax_snapshot_test)
    add_executable(ilic_semantic_snapshot_test test/core/SemanticSnapshotTest.cpp)
    target_link_libraries(ilic_semantic_snapshot_test PRIVATE ilic-core)
    add_test(NAME ilic_semantic_snapshot COMMAND ilic_semantic_snapshot_test)
    add_executable(ilic_formatter_test test/core/FormatterTest.cpp)
    target_link_libraries(ilic_formatter_test PRIVATE ilic-core)
    add_test(NAME ilic_formatter COMMAND ilic_formatter_test)
    if(TARGET ilic-repository)
        add_executable(ilic_repository_test test/repository/RepositoryTest.cpp)
        target_link_libraries(ilic_repository_test PRIVATE ilic-repository)
        add_test(NAME ilic_repository COMMAND ilic_repository_test
            "${CMAKE_CURRENT_SOURCE_DIR}/test/repository/fixture")
        add_test(NAME ilic_repository_cli COMMAND ilic -silent
            -repositories "${CMAKE_CURRENT_SOURCE_DIR}/test/repository/fixture"
            -models RepositoryRoot)
    endif()

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
        PASS_REGULAR_EXPRESSION "platform=(win|macos|wasm|linux|unix)(32|64)"
    )

    add_test(
        NAME ilic_compile_json
        COMMAND "${CMAKE_COMMAND}"
            "-DILIC=$<TARGET_FILE:ilic>"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/compile-json-test"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_compile_json_test.cmake"
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
    add_ilic_regression_test(ilic_regression_set_constraint_structure_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/set_constraint_structure_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_unique_multivalue_attribute_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/unique_multivalue_attribute_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_unique_multivalue_role_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/unique_multivalue_role_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_unique_path_cardinality_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili23/unique_path_cardinality_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_unique_basket_local_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/unique_basket_local_invalid.ili"
    )
    add_ilic_regression_test(ilic_regression_unique_basket_global_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/unique_basket_global_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_generic_contexts_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_contexts_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_generic_context_transitive_import_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_context_transitive_import_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_generic_deferred_nested_valid TRUE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_deferred_nested_valid.ili"
    )
    add_ilic_regression_test(ilic_regression_generic_context_non_generic_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_context_non_generic_invalid.ili"
        OUTPUT_REGEX "requires a GENERIC coordinate domain"
    )
    add_ilic_regression_test(ilic_regression_generic_context_dimension_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_context_dimension_invalid.ili"
        OUTPUT_REGEX "not a compatible specialization"
    )
    add_ilic_regression_test(ilic_regression_generic_context_range_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_context_range_invalid.ili"
        OUTPUT_REGEX "not a compatible specialization"
    )
    add_ilic_regression_test(ilic_regression_generic_context_refinement_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_context_refinement_invalid.ili"
        OUTPUT_REGEX "does not extend a concrete domain from the imported context"
    )
    add_ilic_regression_test(ilic_regression_generic_context_missing_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_context_missing_invalid.ili"
        OUTPUT_REGEX "has no visible context"
    )
    add_ilic_regression_test(ilic_regression_generic_deferred_missing_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_deferred_missing_invalid.ili"
        OUTPUT_REGEX "must be listed in DEFERRED GENERICS"
    )
    add_ilic_regression_test(ilic_regression_generic_deferred_unused_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_deferred_unused_invalid.ili"
        OUTPUT_REGEX "is not used by topic"
    )
    add_ilic_regression_test(ilic_regression_generic_deferred_unknown_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_deferred_unknown_invalid.ili"
        OUTPUT_REGEX "type UnknownPosition not found"
    )
    add_ilic_regression_test(ilic_regression_generic_nested_deferred_missing_invalid FALSE
        INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_nested_deferred_missing_invalid.ili"
        OUTPUT_REGEX "must be listed in DEFERRED GENERICS"
    )
    add_test(
        NAME ilic_regression_generic_roundtrip
        COMMAND "${CMAKE_COMMAND}"
            "-DILIC=$<TARGET_FILE:ilic>"
            "-DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/test/regression/ili24/generic_deferred_nested_valid.ili"
            "-DOUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/generic-roundtrip.ili"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_ilic_generic_roundtrip.cmake"
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
