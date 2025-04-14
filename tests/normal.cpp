#include <gtest/gtest.h>

#include "res-cpp/res-cpp.h"

namespace ResCpp {
TEST(Result, Error) {
    auto testError = []() -> Result<void> {
        return { RESULT_ERROR(), "some error: {0}", 2345 };
    };

    const auto resultTestError = testError();

    EXPECT_TRUE(resultTestError.HasError());
    EXPECT_EQ(resultTestError.Error().str(), "some error: 2345");
}

TEST(Result, TRY) {
    auto testError = []() -> Result<int> {
        return { RESULT_ERROR(), "some error: {0}", 893745 };
    };

    auto testErrorPropagation = [&testError]() -> Result<void> {
        int foo = TRY(testError());
        return { RESULT_ERROR(), "different error: {0}", foo };
    };

    const auto resultTestErrorPropagation = testErrorPropagation();

    EXPECT_EQ(resultTestErrorPropagation.Error().str(), "some error: 893745");
}

TEST(Result, Void) {
    auto testSuccess = []() -> Result<void> {
        return {};
    };

    const auto resultTestSuccess = testSuccess();

    EXPECT_FALSE(resultTestSuccess.HasError());
}

TEST(Result, Value) {
    auto testValue = []() -> Result<int> {
        // also testing convertion
        // from type T2_ (aka 'float') to type T_ (aka 'int') 
        return static_cast<int>(123.5f);
    };

    const auto resultValue = testValue();

    EXPECT_EQ(resultValue.Value(), 123);
}

TEST(Result, LValueReference) {
    int testInt = 3456;
    auto testReference = [&testInt]() -> Result<int&> {
        return testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.Value(), testInt);
}

TEST(Result, ConstLValueReference) {
    constexpr int testInt = 786345;
    auto testReference = [&testInt]() -> Result<const int&> {
        return testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.Value(), testInt);
}

TEST(Result, RValueReference) {
    int testInt = 3456;
    auto testReference = [&testInt]() -> Result<int&&> {
        return std::move(testInt);
    };

    EXPECT_EQ(testReference().Value(), testInt);
}

TEST(Result, ConstRValueReference) {
    constexpr int testInt = 786345;
    auto testReference = [&testInt]() -> Result<const int&&> {
        return std::move(testInt);
    };

    EXPECT_EQ(testReference().Value(), testInt);
}

TEST(Result, Pointer) {
    int testInt = 87345;

    auto testReference = [&testInt]() -> Result<int*> {
        return &testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.Value(), &testInt);
}

TEST(Result, ConstPointer) {
    constexpr int testInt = 12893;

    auto testReference = [&testInt]() -> Result<const int*> {
        return &testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.Value(), &testInt);
}

TEST(Result, Convertion) {
    constexpr float testFloat = 123.5f;

    auto testConvertion = [&testFloat]() -> Result<int> {
        return testFloat;
    };

    const auto resultConvertion = testConvertion();

    EXPECT_EQ(resultConvertion.Value(), static_cast<int>(testFloat));
}
}
