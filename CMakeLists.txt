cmake_minimum_required(VERSION 3.10)
project ( ShaderGraph )
set ( CMAKE_C_STANDARD 11 )
set ( CMAKE_C_STANDARD_REQUIRED ON )

if ( NOT TARGET Platform )
    include ( FetchContent )
    FetchContent_Declare (
        Platform
        GIT_REPOSITORY git@github.com:jpincho/Platform
        GIT_TAG main
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/Platform
        )

    FetchContent_MakeAvailable ( Platform )
endif()

file ( GLOB SHADER_GRAPH_SOURCES "*.c" "*.h" "Nodes/*.c" "Nodes/*.h" )
add_library ( ShaderGraph ${SHADER_GRAPH_SOURCES} )
target_include_directories ( ShaderGraph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )
target_link_libraries ( ShaderGraph PUBLIC Platform )

option ( SHADER_GRAPH_BUILD_TESTS "Build ShaderGraph tests" TRUE )

if ( SHADER_GRAPH_BUILD_TESTS )
    enable_testing()
    file ( GLOB SHADER_GRAPH_TEST_SOURCES "Tests/*.c" )
    foreach ( TEST_SOURCE ${SHADER_GRAPH_TEST_SOURCES} )
        get_filename_component ( TEST_NAME ${TEST_SOURCE} NAME_WE )
        add_executable ( ${TEST_NAME} ${TEST_SOURCE} )
        target_link_libraries ( ${TEST_NAME} PRIVATE ShaderGraph )
        add_test ( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
    endforeach()
endif()
