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

add_library(res-cpp INTERFACE include/res-cpp/res-cpp.h
        include/res-cpp/result.h
        include/res-cpp/formatted_error.h
        include/res-cpp/tags.h
        include/res-cpp/config.h
        include/res-cpp/reference_wrapper.h
        include/res-cpp/type_traits.h
        include/res-cpp/result_storage.h
        include/res-cpp/result_error_base.h
        include/res-cpp/result_error.h
        include/res-cpp/try.h
        include/res-cpp/result_base.h
)
target_link_libraries(res-cpp INTERFACE
        fmt::fmt
)

target_include_directories(res-cpp INTERFACE
        include
)
target_precompile_headers(res-cpp INTERFACE
        include/res-cpp/res-cpp.h
)

if (ENABLE_RESCPP_TESTS)
    add_subdirectory(tests)
endif ()
