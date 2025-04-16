cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 23)

include(cmake/CPM.cmake)

project(res-cpp)

CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 11.1.4
        GIT_SHALLOW ON
        OPTIONS
        "FMT_INSTALL OFF"
)

add_library(res-cpp INTERFACE include/res-cpp/res-cpp.hpp)
target_link_libraries(res-cpp INTERFACE
        fmt::fmt
)
target_include_directories(res-cpp INTERFACE
        include
)
target_precompile_headers(res-cpp INTERFACE
        include/res-cpp/res-cpp.hpp
)

if (ENABLE_RESCPP_TESTS)
    add_subdirectory(tests)
endif ()
