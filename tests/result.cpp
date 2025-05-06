#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <res-cpp/res-cpp.hpp>

#include "catch2/internal/catch_windows_h_proxy.hpp"

// Custom error types for testing
struct TestError {
    int code;
    std::string message;

    TestError(int c, std::string msg) : code(c), message(std::move(msg)) {}

    bool operator==(const TestError& other) const {
        return code == other.code && message == other.message;
    }
};

struct OtherError {
    std::string reason;

    explicit OtherError(std::string r) : reason(std::move(r)) {}

    bool operator==(const OtherError& other) const {
        return reason == other.reason;
    }
};

// Type converter for error conversion testing
template <>
struct rescpp::type_converter<TestError, OtherError> {
    static OtherError convert(const TestError& error) noexcept {
        return OtherError("Converted: " + error.message);
    }
};

TEST_CASE("Basic exception support check", "[setup]") {
    bool exception_caught = false;
    try {
        throw std::runtime_error("Test exception");
    } catch (const std::runtime_error&) {
        exception_caught = true;
    }
    
    INFO("Runtime exception support result: " << (exception_caught ? "WORKING" : "FAILING"));
    CHECK(exception_caught);
}

TEST_CASE("Result basic value functionality", "[result]") {
    SECTION("Construction with value") {
        rescpp::result<int, TestError> res = 42;

        REQUIRE_FALSE(res.has_error());
        REQUIRE(res.value() == 42);
    }

    SECTION("In-place construction") {
        rescpp::result<std::string, TestError> res(std::in_place, "test string");

        REQUIRE_FALSE(res.has_error());
        REQUIRE(res.value() == "test string");
    }

    SECTION("Value conversion constructor") {
        rescpp::result<int, TestError> res = 42;
        rescpp::result<double, TestError> converted = res;

        REQUIRE_FALSE(converted.has_error());
        REQUIRE(converted.value() == 42.0);
    }

    SECTION("Copy construction") {
        rescpp::result<std::string, TestError> res = "original";
        auto copy = res;

        REQUIRE_FALSE(copy.has_error());
        REQUIRE(copy.value() == "original");
    }

    SECTION("Move construction") {
        rescpp::result<std::unique_ptr<int>, TestError> res(std::in_place, new int(42));
        auto moved = std::move(res);

        REQUIRE_FALSE(moved.has_error());
        REQUIRE(*moved.value() == 42);
    }
}

TEST_CASE("Result with references", "[result]") {
    SECTION("Lvalue reference") {
        int value = 42;
        rescpp::result<int&, TestError> res = value;

        REQUIRE_FALSE(res.has_error());
        REQUIRE(&res.value() == &value);

        // Verify modification works through reference
        res.value() = 100;
        REQUIRE(value == 100);
    }

    SECTION("Const reference") {
        const int value = 42;
        rescpp::result<const int&, TestError> res = value;

        REQUIRE_FALSE(res.has_error());
        REQUIRE(&res.value() == &value);
    }
}

TEST_CASE("Result with void value type", "[result]") {
    SECTION("Successful void result") {
        rescpp::result<void, TestError> res;

        REQUIRE_FALSE(res.has_error());
    }

    SECTION("Failed void result") {
        rescpp::result<void, TestError> res =
            rescpp::fail<TestError>(1, "operation failed");

        REQUIRE(res.has_error());
        REQUIRE(res.error().code == 1);
        REQUIRE(res.error().message == "operation failed");
    }
}

TEST_CASE("Failure handling", "[failure]") {
    SECTION("Creating failure") {
        auto failure = rescpp::fail<TestError>(1, "test error");

        REQUIRE(failure.error().code == 1);
        REQUIRE(failure.error().message == "test error");
    }

    SECTION("Converting failure to result") {
        auto failure = rescpp::fail<TestError>(1, "test error");
        rescpp::result<int, TestError> res = failure;

        REQUIRE(res.has_error());
        REQUIRE(res.error().code == 1);
        REQUIRE(res.error().message == "test error");
    }

    SECTION("Error conversion through type_converter") {
        auto failure = rescpp::fail<TestError>(1, "test error");
        rescpp::result<int, OtherError> res = failure;

        REQUIRE(res.has_error());
        REQUIRE(res.error().reason == "Converted: test error");
    }
}

TEST_CASE("Error handling", "[error]") {
    SECTION("Accessing error on good result") {
        rescpp::result<int, TestError> res = 42;

#if defined(RESCPP_DISABLE_EXCEPTIONS) || defined(NDEBUG)
        SKIP("Exceptions are disabled");
#else
        // this should throw
        REQUIRE_THROWS_AS(res.error(), rescpp::detail::bad_result_access_exception);
#endif
    }

    SECTION("Accessing value on bad result") {
        rescpp::result<int, TestError> res = rescpp::fail<TestError>(1, "test error");

#if defined(RESCPP_DISABLE_EXCEPTIONS) || defined(NDEBUG)
        SKIP("Exceptions are disabled");
#else
        // this should throw
        REQUIRE_THROWS_AS(res.value(), rescpp::detail::bad_result_access_exception);
#endif
    }
}

// Helper functions for testing RESCPP_TRY macros
rescpp::result<int, TestError> get_success() {
    return 42;
}

rescpp::result<int, TestError> get_failure() {
    return rescpp::fail<TestError>(1, "operation failed");
}

rescpp::result<void, TestError> do_something_void_success() {
    return {};
}

rescpp::result<void, TestError> do_something_void_failure() {
    return rescpp::fail<TestError>(1, "void operation failed");
}

rescpp::result<int, TestError> chain_operations() {
    RESCPP_TRY(get_success());
    return 100; // This should be reachable
}

rescpp::result<int, TestError> chain_failing_operations() {
    RESCPP_TRY(get_failure());
    return 100; // This should NOT be reachable
}

rescpp::result<int, TestError> chain_operations_with_void() {
    RESCPP_TRY(do_something_void_success());
    return 100; // This should be reachable
}

rescpp::result<int, TestError> chain_failing_operations_with_void() {
    RESCPP_TRY(do_something_void_failure());
    return 100; // This should NOT be reachable
}

// Test the named version of the macro
rescpp::result<int, TestError> named_try_operation() {
    RESCPP_TRY_(value, get_success());
    return value * 2; // This should use the extracted value
}

rescpp::result<int, TestError> named_try_failing_operation() {
    RESCPP_TRY_(value, get_failure());
    return value * 2; // This should NOT be reachable
}

TEST_CASE("RESCPP_TRY macro", "[macros]") {
    SECTION("Successful operation") {
        auto res = chain_operations();

        REQUIRE_FALSE(res.has_error());
        REQUIRE(res.value() == 100);
    }

    SECTION("Failing operation") {
        auto res = chain_failing_operations();

        REQUIRE(res.has_error());
        REQUIRE(res.error().code == 1);
        REQUIRE(res.error().message == "operation failed");
    }

    SECTION("Successful void operation") {
        auto res = chain_operations_with_void();

        REQUIRE_FALSE(res.has_error());
        REQUIRE(res.value() == 100);
    }

    SECTION("Failing void operation") {
        auto res = chain_failing_operations_with_void();

        REQUIRE(res.has_error());
        REQUIRE(res.error().code == 1);
        REQUIRE(res.error().message == "void operation failed");
    }
}

TEST_CASE("RESCPP_TRY_ named macro", "[macros]") {
    SECTION("Successful operation with named result") {
        auto res = named_try_operation();

        REQUIRE_FALSE(res.has_error());
        REQUIRE(res.value() == 84); // 42 * 2
    }

    SECTION("Failing operation with named result") {
        auto res = named_try_failing_operation();

        REQUIRE(res.has_error());
        REQUIRE(res.error().code == 1);
        REQUIRE(res.error().message == "operation failed");
    }
}

// Test more complex scenarios
struct Resource {
    bool initialized = false;

    void initialize() {
        initialized = true;
    }

    void cleanup() {
        initialized = false;
    }
};

rescpp::result<Resource, TestError> create_resource() {
    Resource r;
    r.initialize();
    return r;
}

rescpp::result<int, TestError> use_resource(Resource& r) {
    if (!r.initialized) {
        return rescpp::fail<TestError>(2, "resource not initialized");
    }
    return 42;
}

rescpp::result<int, TestError> resource_operation() {
    RESCPP_TRY_(resource, create_resource());

    // The scope of 'resource' is the rest of the function
    REQUIRE(resource.initialized);

    auto result = RESCPP_TRY(use_resource(resource));
    resource.cleanup();
    return result;
}

TEST_CASE("Complex resource management with TRY macros", "[macros][complex]") {
    auto res = resource_operation();

    REQUIRE_FALSE(res.has_error());
    REQUIRE(res.value() == 42);
}

// Test custom error propagation with TRY macros
rescpp::result<int, OtherError> convert_error_type() {
    // This will fail with TestError, but should convert to OtherError
    auto result = get_failure();
    if (result.has_error()) {
        return rescpp::fail<OtherError>("Converted: " + result.error().message);
    }
    return result.value();
}

TEST_CASE("Custom error propagation", "[macros][error]") {
    auto res = convert_error_type();

    REQUIRE(res.has_error());
    REQUIRE(res.error().reason == "Converted: operation failed");
}

// Test move semantics with unique resources
rescpp::result<std::unique_ptr<int>, TestError> create_unique() {
    return std::make_unique<int>(42);
}

rescpp::result<int, TestError> use_unique() {
    RESCPP_TRY_(ptr, create_unique());
    return *ptr; // ptr is a std::unique_ptr<int>
}

TEST_CASE("Move semantics with TRY macros", "[macros][move]") {
    auto res = use_unique();

    REQUIRE_FALSE(res.has_error());
    REQUIRE(res.value() == 42);
}

// Test constexpr support
constexpr rescpp::result<int, int> constexpr_success() {
    return 42;
}

constexpr rescpp::result<int, int> constexpr_failure() {
    return rescpp::fail<int>(1);
}

TEST_CASE("Constexpr support", "[constexpr]") {
    constexpr auto success = constexpr_success();
    constexpr auto failure = constexpr_failure();

    static_assert(!success.has_error(), "constexpr success check failed");
    static_assert(success.value() == 42, "constexpr value check failed");

    static_assert(failure.has_error(), "constexpr failure check failed");
    static_assert(failure.error() == 1, "constexpr error check failed");

    REQUIRE_FALSE(success.has_error());
    REQUIRE(success.value() == 42);

    REQUIRE(failure.has_error());
    REQUIRE(failure.error() == 1);
}
