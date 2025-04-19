#include <gtest/gtest.h>

#include "res-cpp/res-cpp.hpp"

namespace ResCpp {
TEST(TRY, ErrorPropagation) {
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

TEST(TRY, Success) {
    auto testSuccess = []() -> Result<int> {
        return 42;
    };

    auto testTrySuccess = [&testSuccess]() -> Result<int> {
        int value = TRY(testSuccess());
        return value + 10;
    };

    const auto result = testTrySuccess();
    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(result.value(), 52);
}

TEST(TRY, NestedTry) {
    auto innerSuccess = []() -> Result<int> {
        return 5;
    };

    auto middleSuccess = [&innerSuccess]() -> Result<int> {
        int val = TRY(innerSuccess());
        return val * 2;
    };

    auto outerFunction = [&middleSuccess]() -> Result<int> {
        int val = TRY(middleSuccess());
        return val + 10;
    };

    const auto result = outerFunction();
    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(result.value(), 20); // 5*2+10
}

TEST(TRY, ErrorInNestedTry) {
    auto innerError = []() -> Result<int> {
        return { RESULT_ERROR(), "inner error" };
    };

    auto middleFunction = [&innerError]() -> Result<int> {
        int val = TRY(innerError());
        return val * 2;
    };

    auto outerFunction = [&middleFunction]() -> Result<int> {
        int val = TRY(middleFunction());
        return val + 10;
    };

    const auto result = outerFunction();
    EXPECT_TRUE(result.has_error());
    EXPECT_EQ(result.error().str(), "inner error");
}

TEST(TRY, WithReferences) {
    int testValue = 50;

    auto refFunction = [&testValue]() -> Result<int&> {
        return testValue;
    };

    auto useReference = [&refFunction]() -> Result<int> {
        int& val = TRY(refFunction());
        val += 25;
        return val;
    };

    const auto result = useReference();
    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(result.value(), 75);
    EXPECT_EQ(testValue, 75); // Verify the original was modified
}

TEST(TRY, WithVoid) {
    auto voidSuccess = []() -> Result<void> {
        return {};
    };

    auto useVoid = [&voidSuccess]() -> Result<int> {
        TRY(voidSuccess());
        return 42;
    };

    const auto result = useVoid();
    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(result.value(), 42);
}

TEST(TRY, VoidError) {
    auto voidError = []() -> Result<void> {
        return { RESULT_ERROR(), "void error" };
    };

    auto useVoidError = [&voidError]() -> Result<int> {
        TRY(voidError());
        return 42;
    };

    const auto result = useVoidError();
    EXPECT_TRUE(result.has_error());
    EXPECT_EQ(result.error().str(), "void error");
}

TEST(TRY, DifferentErrorTypes) {
    struct CustomError : ResultErrorBase<CustomError> {
        std::string message;

        explicit CustomError(std::string msg)
            : message(std::move(msg)) {}
        
        CustomError& operator=(const CustomError& other) noexcept {
            if (this == &other) {
                return *this;
            }

            try {
                message = other.message;
            }
            catch (const std::exception& e) {
                message = e.what();
            }

            return *this;
        }

        void print(std::ostream& stream) const noexcept override {
            stream << message;
        }

        [[nodiscard]] std::string str() const noexcept override {
            return message;
        }
    };

    auto customErrorFunc = []() -> Result<int, CustomError> {
        return { RESULT_ERROR(), CustomError("custom error type") };
    };

    auto useCustomError = [&customErrorFunc]() -> Result<int> {
        try {
            auto result = customErrorFunc();
            if (result.has_error()) {
                return { RESULT_ERROR(), result.error() };
            }
            return result.value();
        }
        catch (const std::exception& e) {
            return { RESULT_ERROR(), "Exception: {}", e.what() };
        }
    };

    const auto result = useCustomError();
    EXPECT_TRUE(result.has_error());
    EXPECT_EQ(result.error().str(), "custom error type");
}
}
