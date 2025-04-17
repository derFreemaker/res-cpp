cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 23)

project(res-cpp_tests)

CPMAddPackage(
        NAME GoogleTest
        VERSION 1.16.0
        GITHUB_REPOSITORY google/googletest
        OPTIONS
        "INSTALL_GTEST OFF"
        "BUILD_GMOCK ON"
)

add_executable(res-cpp_tests
        tracking_struct.hpp

        normal.cpp
        try.cpp
        hold.cpp
        common.hpp
)
target_link_libraries(res-cpp_tests PUBLIC
        gtest
        gtest_main
        gmock
        res-cpp
)
