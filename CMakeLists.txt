cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

project(res-cpp)

add_library(res-cpp INTERFACE include/res-cpp/res-cpp.hpp)
target_precompile_headers(res-cpp INTERFACE include/res-cpp/res-cpp.hpp)

if (RESCPP_ENABLE_TESTS)
    add_subdirectory(tests)
endif ()