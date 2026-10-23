cmake_minimum_required(VERSION 3.10)
project ( ShaderGraph )
set ( CMAKE_C_STANDARD 11 )
set ( CMAKE_C_STANDARD_REQUIRED ON )
include ( target_warning_options.cmake )

if ( NOT TARGET Platform )
    include ( FetchContent )
    FetchContent_Declare (
        Platform
        GIT_REPOSITORY https://github.com/jpincho/Platform
        GIT_TAG main
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/Platform
        )

    FetchContent_MakeAvailable ( Platform )
endif()

find_package(glslang)
if ( glslang_FOUND )
    message ( STATUS "${PROJECT_NAME} - GLSLang found")
endif()

file ( GLOB SHADER_GRAPH_SOURCES "*.c" "*.h" )
file ( GLOB GLSL_GENERATOR_SOURCES "GLSLGenerator/*" )
add_library ( ShaderGraph ${SHADER_GRAPH_SOURCES} ${GLSL_GENERATOR_SOURCES} )
target_include_directories ( ShaderGraph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )
target_link_libraries ( ShaderGraph PUBLIC Platform )
target_warning_options ( ShaderGraph HIGH AS_ERRORS )

option ( SHADER_GRAPH_BUILD_TESTS "Build ShaderGraph tests" TRUE )
if ( SHADER_GRAPH_BUILD_TESTS )
    enable_testing()
    file ( GLOB SHADER_GRAPH_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/Tests/*.c" )
    file ( GLOB SHADER_GRAPH_COMMON_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/Tests/TestUtils/*" )
    foreach ( TEST_SOURCE ${SHADER_GRAPH_TEST_SOURCES} )
        get_filename_component ( TEST_NAME ${TEST_SOURCE} NAME_WE )
        add_executable ( ${TEST_NAME} ${TEST_SOURCE} ${SHADER_GRAPH_COMMON_TEST_SOURCES} )
        target_warning_options ( ${TEST_NAME} HIGH AS_ERRORS )
        target_link_libraries ( ${TEST_NAME} PRIVATE ShaderGraph )
        message ( STATUS "${PROJECT_NAME} - Building test ${TEST_NAME}")
        if(glslang_FOUND)
            target_link_libraries ( ${TEST_NAME} PRIVATE glslang::glslang glslang::glslang-default-resource-limits )
            target_compile_definitions(${TEST_NAME} PRIVATE GLSLANG_ENABLED )
        endif()
        add_test ( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
    endforeach()
endif()
