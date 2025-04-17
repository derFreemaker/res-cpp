#ifndef RESCPP_TESTS_COMMON_HPP
#define RESCPP_TESTS_COMMON_HPP

#include <res-cpp/res-cpp.hpp>

#define TRY_FAIL(...) \
    TRY_IMPL((__VA_ARGS__), { \
        FAIL() << __result__.error().str(); \
    })

#endif //RESCPP_TESTS_COMMON_HPP
