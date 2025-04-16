#include <gtest/gtest.h>

#include "res-cpp/res-cpp.hpp"

namespace ResCpp {
TEST(Result, TRY) {
    auto testError = []() -> Result<int> {
        return { RESULT_ERROR(), "some error: {0}", 893745 };
    };

    auto testErrorPropagation = [&testError]() -> Result<void> {
        int foo = TRY(testError());
        return { RESULT_ERROR(), "different error: {0}", foo };
    };

    const auto resultTestErrorPropagation = testErrorPropagation();

    EXPECT_EQ(resultTestErrorPropagation.error().str(), "some error: 893745");
}

//TODO: add more tests

}
