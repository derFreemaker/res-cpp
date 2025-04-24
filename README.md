# res-cpp
A simple header-only, single file result type for c++.

# Personal Project
This is a personal project, meaning I don't bother with some more optional things
like big compiler support or as low as possible c++ standard.

If any interest ever should arise in this "project" please open an issue with what you need,
and we go from there.

# Features
- support for any return type
- support for custom error type (no references or pointers currently)
- lightweight (complete 'constexpr')
- auto convertion from one result type to other (if possible)

# TODO
- more compiler support
- lower needed c++ standard

# Dependencies (only Testing)
getting managed through [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake)

- catch2
