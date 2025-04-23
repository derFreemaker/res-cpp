cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

project(res-cpp_tests)

include(../cmake/CPM.cmake)

CPMAddPackage(
        NAME Catch2
        VERSION 3.8.1
        GITHUB_REPOSITORY catchorg/Catch2
)

add_executable(res-cpp_tests
        result.cpp
)
target_link_libraries(res-cpp_tests
        Catch2::Catch2WithMain
        res-cpp
)

target_compile_definitions(res-cpp INTERFACE
        RESCPP_DISABLE_TRY_MACRO
)
