cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

project(res-cpp)

add_library(res-cpp INTERFACE include/res-cpp/res-cpp.hpp)
target_precompile_headers(res-cpp INTERFACE include/res-cpp/res-cpp.hpp)
target_include_directories(res-cpp INTERFACE include)

option(RESCPP_DISABLE_EXCEPTIONS "disables exceptions instead will call abort")
if (${RESCPP_DISABLE_EXCEPTIONS})
    target_compile_definitions(res-cpp INTERFACE
            RESCPP_DISABLE_EXCEPTIONS
    )
endif ()

option(RESCPP_DISABLE_CHECKS_IN_RELEASE "sets 'RESCPP_DISABLE_CHECKS' in release")
if (${RESCPP_DISABLE_CHECKS_IN_RELEASE})
    target_compile_definitions(res-cpp INTERFACE
            RESCPP_DISABLE_CHECKS_IN_RELEASE
    )
endif ()

option(RESCPP_DISABLE_CHECKS "disables result state check when calling .value() or .error() (performance, optimization)")
if (${RESCPP_DISABLE_CHECKS})
    target_compile_definitions(res-cpp INTERFACE
            RESCPP_DISABLE_CHECKS
    )
endif ()

option(RESCPP_DISABLE_TRY_MACROS "disables try macros")
if (${RESCPP_DISABLE_TRY_MACROS})
    target_compile_definitions(res-cpp INTERFACE
            RESCPP_DISABLE_TRY_MACRO
    )
endif ()

if (RESCPP_ENABLE_TESTS)
    add_subdirectory(tests)
endif ()

if (RESCPP_ENABLE_EXAMPLE)
    add_subdirectory(example)
endif ()