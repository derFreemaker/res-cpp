cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

project(res-cpp_example)

add_executable(res-cpp_example
        example.cpp
)
target_link_libraries(res-cpp_example
        res-cpp
)
