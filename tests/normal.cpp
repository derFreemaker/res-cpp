#include <array>
#include <gtest/gtest.h>

#include "common.hpp"

namespace ResCpp::testing {
TEST(Normal, Error) {
    auto testError = []() -> Result<void> {
        return { RESULT_ERROR(), "some error: {0}", 2345 };
    };

    const auto resultTestError = testError();

    EXPECT_TRUE(resultTestError.has_error());
    EXPECT_EQ(resultTestError.error().str(), "some error: 2345");
}

TEST(Normal, Void) {
    auto testSuccess = []() -> Result<void> {
        return {};
    };

    const auto resultTestSuccess = testSuccess();

    EXPECT_FALSE(resultTestSuccess.has_error());
}

TEST(Normal, Value) {
    {
        auto testValue = []() -> Result<TrackingStruct<int>> {
            return TrackingStruct(123);
        };

        const auto resultValue = TRY_FAIL(testValue());

        EXPECT_EQ(resultValue.value, 123);
    }

    FAIL_TRACKING_HAS_OPERATION(DefaultConstructor)
    FAIL_TRACKING_HAS_OPERATION(CopyAssignment)
    FAIL_TRACKING_HAS_OPERATION(MoveAssignment)

    FAIL_TRACKING_HAS_MORE_OPERATION(ValueConstructor, 1)
    FAIL_TRACKING_HAS_MORE_OPERATION(MoveConstructor, 1)
    FAIL_TRACKING_HAS_MORE_OPERATION(CopyConstructor, 1)
    FAIL_TRACKING_HAS_MORE_OPERATION(Destructor, 2)
}

TEST(Normal, LValueReference) {
    int testInt = 3456;
    auto testReference = [&testInt]() -> Result<int&> {
        return testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.value(), testInt);
}

TEST(Normal, ConstLValueReference) {
    constexpr int testInt = 786345;
    auto testReference = [&testInt]() -> Result<const int&> {
        return testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.value(), testInt);
}

TEST(Normal, RValueReference) {
    int testInt = 3456;
    auto testReference = [&testInt]() -> Result<int&&> {
        return std::move(testInt); // NOLINT(*-move-const-arg)
    };

    EXPECT_EQ(testReference().value(), testInt);
}

TEST(Normal, ConstRValueReference) {
    constexpr int testInt = 786345;
    auto testReference = [&testInt]() -> Result<const int&&> {
        return std::move(testInt); // NOLINT(*-move-const-arg)
    };

    EXPECT_EQ(testReference().value(), testInt);
}

TEST(Normal, Pointer) {
    int testInt = 87345;

    auto testReference = [&testInt]() -> Result<int*> {
        return &testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.value(), &testInt);
}

TEST(Normal, ConstPointer) {
    constexpr int testInt = 12893;

    auto testReference = [&testInt]() -> Result<const int*> {
        return &testInt;
    };

    const auto resultReference = testReference();

    EXPECT_EQ(resultReference.value(), &testInt);
}

TEST(Normal, Convertion) {
    constexpr float testFloat = 123.5f;

    auto testConvertion = [&testFloat]() -> Result<int> {
        return testFloat;
    };

    const auto resultConvertion = testConvertion();

    EXPECT_EQ(resultConvertion.value(), static_cast<int>(testFloat));
}

// Testing a chain of Results
TEST(Normal, Chain) {
    auto firstFunction = []() -> Result<int> {
        return 42;
    };

    auto secondFunction = [](int value) -> Result<std::string> {
        return std::to_string(value * 2);
    };

    auto chainResults = [&firstFunction, &secondFunction]() -> Result<std::string> {
        auto first = firstFunction();
        if (first.has_error()) {
            return first;
        }
        return secondFunction(first.value());
    };

    const auto result = chainResults();
    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(result.value(), "84");
}

// Testing with custom error types
TEST(Normal, CustomErrorType) {
    struct CustomError : ResultErrorBase<CustomError> {
        int code;
        std::string message;

        CustomError(int c, std::string msg)
            : code(c), message(std::move(msg)) {}

        CustomError& operator=(const CustomError&) noexcept = default;

        void print(std::ostream& stream) const noexcept override {
            stream << "Error " << code << ": " << message;
        }

        [[nodiscard]] std::string str() const noexcept override {
            std::stringstream ss;
            print(ss);
            return ss.str();
        }
    };

    auto testCustomError = []() -> Result<int, CustomError> {
        return { RESULT_ERROR(), CustomError(404, "Not Found") };
    };

    const auto result = testCustomError();
    EXPECT_TRUE(result.has_error());
    EXPECT_EQ(result.error().str(), "Error 404: Not Found");
}

// Testing with std::tuple
TEST(Normal, Tuple) {
    auto tupleFunction = []() -> Result<std::tuple<int, std::string, double>> {
        return std::make_tuple(42, std::string("hello"), 3.14);
    };

    const auto result = tupleFunction();
    EXPECT_FALSE(result.has_error());

    auto [num, str, dbl] = result.value();
    EXPECT_EQ(num, 42);
    EXPECT_EQ(str, "hello");
    EXPECT_DOUBLE_EQ(dbl, 3.14);
}

// Testing const correctness
TEST(Normal, ConstCorrectness) {
    auto getValue = []() -> Result<int> {
        return 42;
    };

    const auto result = getValue();
    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(result.value(), 42);

    // Test rvalue access on const object
    EXPECT_EQ(std::move(result).value(), 42);
}

// Testing with move-only types
TEST(Normal, MoveOnlyTypes) {
    auto getUniquePtr = []() -> Result<std::unique_ptr<int>> {
        return std::make_unique<int>(42);
    };

    auto result = getUniquePtr();
    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(*result.value(), 42);

    // Taking ownership of the unique_ptr
    std::unique_ptr<int> ptr = std::move(result.value());
    EXPECT_EQ(*ptr, 42);
}

// Testing with large objects to check performance with stack vs heap storage
TEST(Normal, LargeObjects) {
    struct LargeObject {
        std::array<char, 1024> data;

        LargeObject()
            : data{} {
            data.fill('A');
        }

        [[nodiscard]]
        bool isValid() const {
            return data[0] == 'A' && data[1023] == 'A';
        }
    };

    auto getLargeObject = []() -> Result<LargeObject> {
        return LargeObject{};
    };

    const auto result = getLargeObject();
    EXPECT_FALSE(result.has_error());
    EXPECT_TRUE(result.value().isValid());
}

// Testing error handling behaviors
TEST(Normal, AccessingErrorOnSuccess) {
    auto getSuccess = []() -> Result<int> {
        return 42;
    };

    const auto result = getSuccess();
    EXPECT_FALSE(result.has_error());
    EXPECT_THROW((void)result.error(), std::logic_error);
}

// Testing thread safety of the thread_local storage
TEST(Normal, ThreadLocalStorage) {
    auto getThreadValue = [](int value) -> Result<int> {
        return value;
    };

    // Create threads that use thread_local storage
    std::vector<std::thread> threads;
    std::vector<int> results(10, 0);

    threads.reserve(10);
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&results, &getThreadValue, i]() {
            const auto result = getThreadValue(i * 10);
            if (!result.has_error()) {
                results[i] = result.value();
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Verify each thread got its own value
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(results[i], i * 10);
    }
}
}
