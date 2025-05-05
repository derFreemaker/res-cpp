#include <string>
#include <utility>

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <res-cpp/res-cpp.hpp>

enum class ErrorCode {
    None,
    InvalidArgument,
    OutOfRange,
    InternalError
};

// Test helper functions that return results
rescpp::result<int, ErrorCode> add_positive(int a, int b) {
    if (a < 0 || b < 0) {
        return rescpp::fail(ErrorCode::InvalidArgument);
    }
    return a + b;
}

rescpp::result<std::string, ErrorCode> int_to_string(int value) {
    if (value < 0) {
        return rescpp::fail(ErrorCode::InvalidArgument);
    }

    return std::to_string(value);
}

rescpp::result<void, ErrorCode> validate_positive(int value) {
    if (value <= 0) {
        return rescpp::fail(ErrorCode::InvalidArgument);
    }

    return {};
}

// Function that uses RESCPP_TRY macro
rescpp::result<int, ErrorCode> calculate_sum_string_length(int a, int b, int c) {
    auto sum = RESCPP_TRY(add_positive(a, b));
    auto str = RESCPP_TRY(int_to_string(sum));
    RESCPP_TRY(validate_positive(c));
    return static_cast<int>(str.length()) + c;
}

// Function that uses RESCPP_TRY_ macro with named variables
rescpp::result<int, ErrorCode> calculate_complex_operation(int a, int b, int c) {
    RESCPP_TRY_(sum, add_positive(a, b));
    RESCPP_TRY_(str, int_to_string(sum));
    RESCPP_TRY_(_, validate_positive(c));
    return static_cast<int>(str.length()) + c;
}

// Custom error type for testing error propagation
struct CustomError {
    std::string message;
    int code;
    
    bool operator==(const CustomError& other) const {
        return message == other.message && code == other.code;
    }
};

// Function using a custom error type
rescpp::result<int, CustomError> multiply_positive(int a, int b) {
    if (a <= 0 || b <= 0) {
        return rescpp::fail(CustomError{"Negative or zero input", 42});
    }
    return a * b;
}

// Function that uses RESCPP_TRY with custom error type
rescpp::result<int, CustomError> calculate_with_custom_error(int a, int b) {
    auto result = RESCPP_TRY(multiply_positive(a, b));
    return result * 2;
}

TEST_CASE("RESCPP_TRY macro with success path", "[try_macro]") {
    auto result = calculate_sum_string_length(10, 20, 5);
    REQUIRE_FALSE(result.has_error());
    REQUIRE(result.value() == 7); // "30" has length 2, plus 5
}

TEST_CASE("RESCPP_TRY macro error propagation", "[try_macro]") {
    SECTION("Error in first call") {
        auto result = calculate_sum_string_length(-10, 20, 5);
        REQUIRE(result.has_error());
        REQUIRE(result.error() == ErrorCode::InvalidArgument);
    }
    
    SECTION("Error in second call") {
        auto result = calculate_sum_string_length(10, 20, -5);
        REQUIRE(result.has_error());
        REQUIRE(result.error() == ErrorCode::InvalidArgument);
    }
}

TEST_CASE("RESCPP_TRY_ macro with named variables", "[try_macro]") {
    SECTION("Success path") {
        auto result = calculate_complex_operation(10, 20, 5);
        REQUIRE_FALSE(result.has_error());
        REQUIRE(result.value() == 7); // "30" has length 2, plus 5
    }
    
    SECTION("Error propagation") {
        auto result = calculate_complex_operation(10, -20, 5);
        REQUIRE(result.has_error());
        REQUIRE(result.error() == ErrorCode::InvalidArgument);
    }
}

TEST_CASE("RESCPP_TRY with custom error types", "[try_macro]") {
    SECTION("Success path") {
        auto result = calculate_with_custom_error(5, 10);
        REQUIRE_FALSE(result.has_error());
        REQUIRE(result.value() == 100); // 5 * 10 * 2
    }
    
    SECTION("Error propagation") {
        auto result = calculate_with_custom_error(-5, 10);
        REQUIRE(result.has_error());
        REQUIRE(result.error() == CustomError{"Negative or zero input", 42});
    }
}

TEST_CASE("RESCPP_TRY with void return type", "[try_macro]") {
    auto test_function = [](int value) -> rescpp::result<int, ErrorCode> {
        RESCPP_TRY(validate_positive(value));
        return value * 2;
    };
    
    SECTION("Success path") {
        auto result = test_function(10);
        REQUIRE_FALSE(result.has_error());
        REQUIRE(result.value() == 20);
    }
    
    SECTION("Error propagation") {
        auto result = test_function(-5);
        REQUIRE(result.has_error());
        REQUIRE(result.error() == ErrorCode::InvalidArgument);
    }
}

TEST_CASE("RESCPP_TRY with nested calls", "[try_macro]") {
    auto nested_function = [](int a, int b, int c) -> rescpp::result<int, ErrorCode> {
        auto sum1 = RESCPP_TRY(add_positive(a, b));
        auto sum2 = RESCPP_TRY(add_positive(sum1, c));
        return sum2;
    };
    
    SECTION("Success path") {
        auto result = nested_function(10, 20, 30);
        REQUIRE_FALSE(result.has_error());
        REQUIRE(result.value() == 60);
    }
    
    SECTION("Error in inner call") {
        auto result = nested_function(10, 20, -30);
        REQUIRE(result.has_error());
        REQUIRE(result.error() == ErrorCode::InvalidArgument);
    }
}

TEST_CASE("RESCPP_TRY with complex return types", "[try_macro]") {
    auto return_complex = [](bool succeed) -> rescpp::result<std::pair<int, std::string>, ErrorCode> {
        if (!succeed) {
            return rescpp::fail(ErrorCode::InternalError);
        }
        return std::make_pair(42, "success");
    };
    
    auto use_complex = [&](bool succeed) -> rescpp::result<std::string, ErrorCode> {
        auto pair = RESCPP_TRY(return_complex(succeed));
        return pair.second + "_" + std::to_string(pair.first);
    };
    
    SECTION("Success path") {
        auto result = use_complex(true);
        REQUIRE_FALSE(result.has_error());
        REQUIRE(result.value() == "success_42");
    }
    
    SECTION("Error propagation") {
        auto result = use_complex(false);
        REQUIRE(result.has_error());
        REQUIRE(result.error() == ErrorCode::InternalError);
    }
}

TEST_CASE("RESCPP_TRY_ macro variable scope", "[try_macro]") {
    auto test_function = []() -> rescpp::result<int, ErrorCode> {
        RESCPP_TRY_(a, add_positive(10, 20));
        RESCPP_TRY_(b, add_positive(a, 30));
        return a + b; // should be 30 + 60 = 90
    };
    
    auto result = test_function();
    REQUIRE_FALSE(result.has_error());
    REQUIRE(result.value() == 90);
}

TEST_CASE("RESCPP_TRY with reference types", "[try_macro]") {
    struct TestObject {
        int value;
        explicit TestObject(int v) : value(v) {}
    };
    
    TestObject obj(42);
    
    auto get_reference = [&]() -> rescpp::result<TestObject&, ErrorCode> {
        return obj;
    };
    
    auto modify_through_try = [&]() -> rescpp::result<int, ErrorCode> {
        auto& ref = RESCPP_TRY(get_reference());
        ref.value *= 2;
        return ref.value;
    };
    
    auto result = modify_through_try();
    REQUIRE_FALSE(result.has_error());
    REQUIRE(result.value() == 84);
    REQUIRE(obj.value == 84); // The original object should be modified
}

