cmake_minimum_required(VERSION 3.20)

project(ilic VERSION 0.9.9 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
