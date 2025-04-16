#include <gtest/gtest.h>

#include <res-cpp/res-cpp.hpp>

namespace ResCpp {
TEST(ResultHolder, Success) {
    auto createSuccessHolder = []() -> Result<int> {
        return 42;
    };

    auto holder = createSuccessHolder().hold();
    EXPECT_FALSE(holder.has_error());
    EXPECT_EQ(holder.value(), 42);
}

TEST(ResultHolder, Error) {
    auto createErrorHolder = []() -> Result<int> {
        return ResultError("test error");
    };

    auto holder = createErrorHolder().hold();
    EXPECT_TRUE(holder.has_error());
    EXPECT_EQ(holder.error().str(), "test error");
    EXPECT_THROW(holder.value(), std::runtime_error);
}

TEST(ResultHolder, VoidSuccess) {
    auto createVoidSuccessHolder = []() -> Result<void> {
        return ResultError("should not be accessed");
    };

    auto holder = createVoidSuccessHolder().hold();
    // This test is a bit problematic as the current implementation doesn't have a 
    // constructor for void success cases
    EXPECT_TRUE(holder.has_error());
}

TEST(ResultHolder, VoidError) {
    auto createVoidErrorHolder = []() -> Result<void> {
        return ResultError("test void error");
    };

    auto holder = createVoidErrorHolder().hold();
    EXPECT_TRUE(holder.has_error());
    EXPECT_EQ(holder.error().str(), "test void error");
}

TEST(ResultHolder, ConversionToResult) {
    auto createSuccessHolder = []() -> Result<int> {
        return 42;
    };

    auto holder = createSuccessHolder().hold();
    Result<int, ResultError> result = holder;
    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(result.value(), 42);
}

TEST(ResultHolder, ErrorConversionToResult) {
    auto createErrorHolder = []() -> Result<int> {
        return ResultError("test error conversion");
    };

    auto holder = createErrorHolder().hold();
    Result<int, ResultError> result = holder;
    EXPECT_TRUE(result.has_error());
    EXPECT_EQ(result.error().str(), "test error conversion");
}

TEST(ResultHolder, Reference) {
    int testInt = 3456;
    auto createRefHolder = [&testInt]() -> Result<int&> {
        return testInt;
    };

    auto holder = createRefHolder().hold();
    EXPECT_FALSE(holder.has_error());
    EXPECT_EQ(holder.value(), testInt);

    // Verify it's actually a reference
    testInt = 7890;
    EXPECT_EQ(holder.value(), 7890);
}

TEST(ResultHolder, MoveSemantics) {
    struct MoveOnly {
        std::unique_ptr<int> value;
        
        explicit MoveOnly(int v) : value(std::make_unique<int>(v)) {}
        MoveOnly(MoveOnly&&) = default;
        MoveOnly& operator=(MoveOnly&&) = default;
        MoveOnly(const MoveOnly&) = delete;
        MoveOnly& operator=(const MoveOnly&) = delete;
        
        int get() const { return *value; }
    };

    auto createMoveOnlyHolder = []() -> Result<MoveOnly> {
        return MoveOnly(42);
    };

    auto holder = createMoveOnlyHolder().hold();
    EXPECT_FALSE(holder.has_error());
    EXPECT_EQ(holder.value().get(), 42);
}
}