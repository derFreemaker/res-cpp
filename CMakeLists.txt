cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

option(RESCPP_DISABLE_EXCEPTIONS "disables exceptions instead will call abort")

project(res-cpp)

add_library(res-cpp INTERFACE include/res-cpp/res-cpp.hpp)
target_precompile_headers(res-cpp INTERFACE include/res-cpp/res-cpp.hpp)
target_include_directories(res-cpp INTERFACE include)

if (${RESCPP_DISABLE_EXCEPTIONS})
    target_compile_definitions(res-cpp
            RESCPP_RESULT_DISABLE_EXCEPTIONS
    )
endif ()

if (RESCPP_ENABLE_TESTS)
    add_subdirectory(tests)
endif ()

if (RESCPP_ENABLE_EXAMPLE)
    add_subdirectory(example)
endif ()