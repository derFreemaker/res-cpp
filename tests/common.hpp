#ifndef RESCPP_TESTS_COMMON_HPP
#define RESCPP_TESTS_COMMON_HPP

#include <res-cpp/res-cpp.hpp>

#include "tracking_struct.hpp"

#define TRY_FAIL(...) \
    TRY_IMPL((__VA_ARGS__), { \
        FAIL() << result_.error(); \
    })

#endif //RESCPP_TESTS_COMMON_HPP
