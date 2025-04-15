#include <gtest/gtest.h>

#include "res-cpp/res-cpp.h"

namespace ResCpp {
TEST(Result, Error) {
    auto testError = []() -> Result<void> {
        return { RESULT_ERROR(), "some error: {0}", 2345 };
    };

    const auto resultTestError = testError();

    EXPECT_TRUE(resultTestError.has_error());
    EXPECT_EQ(resultTestError.error().str(), "some error: 2345");
}

TEST(Result, Void) {
    auto testSuccess = []() -> Result<void> {
        return {};
    };

    const auto resultTestSuccess = testSuccess();

    EXPECT_FALSE(resultTestSuccess.has_error());
}

TEST(Result, Value) {
    auto testValue = []() -> Result<int> {
        // also testing convertion
        // from type T2_ (aka 'float') to type T_ (aka 'int') 
        return static_cast<int>(123.5f);
    };

    const auto resultValue = testValue();

    EXPECT_EQ(resultValue.value(), 123);
}

TEST(Result, LValueReference) {
    int testInt = 3456;
    auto testReference = [&testInt]() -> Result<int&> {
        return testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.value(), testInt);
}

TEST(Result, ConstLValueReference) {
    constexpr int testInt = 786345;
    auto testReference = [&testInt]() -> Result<const int&> {
        return testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.value(), testInt);
}

TEST(Result, RValueReference) {
    int testInt = 3456;
    auto testReference = [&testInt]() -> Result<int&&> {
        return std::move(testInt);
    };

    EXPECT_EQ(testReference().value(), testInt);
}

TEST(Result, ConstRValueReference) {
    constexpr int testInt = 786345;
    auto testReference = [&testInt]() -> Result<const int&&> {
        return std::move(testInt);
    };

    EXPECT_EQ(testReference().value(), testInt);
}

TEST(Result, Pointer) {
    int testInt = 87345;

    auto testReference = [&testInt]() -> Result<int*> {
        return &testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.value(), &testInt);
}

TEST(Result, ConstPointer) {
    constexpr int testInt = 12893;

    auto testReference = [&testInt]() -> Result<const int*> {
        return &testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.value(), &testInt);
}

TEST(Result, Convertion) {
    constexpr float testFloat = 123.5f;

    auto testConvertion = [&testFloat]() -> Result<int> {
        return testFloat;
    };

    const auto resultConvertion = testConvertion();

    EXPECT_EQ(resultConvertion.value(), static_cast<int>(testFloat));
}
}
