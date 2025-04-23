#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <functional>

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "res-cpp/res-cpp.hpp"

//TODO: write better tests

// Test that various types can be stored in result<T, E>
TEST_CASE("Result compatibility with different value types", "[result][types]") {
    SECTION("Fundamental types") {
        // Integer types
        REQUIRE_NOTHROW((rescpp::result<int, std::string>(42)));
        REQUIRE_NOTHROW((rescpp::result<unsigned int, std::string>(42u)));
        REQUIRE_NOTHROW((rescpp::result<short, std::string>(42)));
        REQUIRE_NOTHROW((rescpp::result<long, std::string>(42L)));
        REQUIRE_NOTHROW((rescpp::result<long long, std::string>(42LL)));

        // Floating point types
        REQUIRE_NOTHROW((rescpp::result<float, std::string>(3.14f)));
        REQUIRE_NOTHROW((rescpp::result<double, std::string>(3.14)));

        // Boolean
        REQUIRE_NOTHROW((rescpp::result<bool, std::string>(true)));

        // Character types
        REQUIRE_NOTHROW((rescpp::result<char, std::string>('A')));
        REQUIRE_NOTHROW((rescpp::result<wchar_t, std::string>(L'A')));
        REQUIRE_NOTHROW((rescpp::result<char16_t, std::string>(u'A')));
        REQUIRE_NOTHROW((rescpp::result<char32_t, std::string>(U'A')));
    }

    SECTION("Standard library types") {
        // String types
        REQUIRE_NOTHROW((rescpp::result<std::string, int>("hello")));
        REQUIRE_NOTHROW((rescpp::result<std::wstring, int>(L"hello")));
        REQUIRE_NOTHROW((rescpp::result<std::u16string, int>(u"hello")));
        REQUIRE_NOTHROW((rescpp::result<std::u32string, int>(U"hello")));

        // Container types
        REQUIRE_NOTHROW((rescpp::result<std::vector<int>, std::string>(std::vector<int>{1, 2, 3})));
        REQUIRE_NOTHROW((rescpp::result<std::array<int, 3>, std::string>(std::array<int, 3>{1, 2, 3})));
        REQUIRE_NOTHROW((rescpp::result<std::map<int, std::string>, std::string>(
            std::map<int, std::string>{{1, "one"}, {2, "two"}})));
        REQUIRE_NOTHROW((rescpp::result<std::unordered_map<int, std::string>, std::string>(
            std::unordered_map<int, std::string>{{1, "one"}, {2, "two"}})));

        // Smart pointers
        REQUIRE_NOTHROW((rescpp::result<std::unique_ptr<int>, std::string>(std::make_unique<int>(42))));
        REQUIRE_NOTHROW((rescpp::result<std::shared_ptr<int>, std::string>(std::make_shared<int>(42))));

        // Optional and variant
        REQUIRE_NOTHROW((rescpp::result<std::optional<int>, std::string>(std::optional<int>(42))));
        REQUIRE_NOTHROW((rescpp::result<std::variant<int, std::string>, std::string>(
            std::variant<int, std::string>("hello"))));
    }

    SECTION("Lvalue Reference types") {
        int value = 42;
        std::string str = "hello";

        // Lvalue references
        REQUIRE_NOTHROW((rescpp::result<int&, std::string>(value)));
        REQUIRE_NOTHROW((rescpp::result<std::string&, int>(str)));

        // Test that references work correctly
        rescpp::result<int&, std::string> r1(value);
        REQUIRE(r1.value() == 42);
        r1.value() = 100;
        REQUIRE(value == 100);

        rescpp::result<std::string&, int> r2(str);
        REQUIRE(r2.value() == "hello");
        r2.value() += " world";
        REQUIRE(str == "hello world");
    }

    SECTION("Rvalue Reference types") {
        std::unique_ptr<int> value = std::make_unique<int>(42);
        std::string str = "hello";

        // Rvalue references
        REQUIRE_NOTHROW((rescpp::result<std::string&&, int>(std::move(str))));

        // Test that references work correctly
        rescpp::result<std::unique_ptr<int>, std::string> r1(std::move(value));
        REQUIRE(*r1.value() == 42);

        rescpp::result<std::string&&, int> r2(std::move(str));
        REQUIRE(r2.value() == "hello");
    }

    SECTION("Pointer types") {
        int value = 42;

        // Raw pointers
        REQUIRE_NOTHROW((rescpp::result<int*, std::string>(&value)));
        REQUIRE_NOTHROW((rescpp::result<const int*, std::string>(&value)));

        // Function pointers
        int (*func_ptr)(int) = [](int x) {
            return x * 2;
        };
        REQUIRE_NOTHROW((rescpp::result<int(*)(int), std::string>(func_ptr)));

        // Test that pointers work correctly
        rescpp::result<int*, std::string> r1(&value);
        REQUIRE(*r1.value() == 42);
        *r1.value() = 100;
        REQUIRE(value == 100);
    }

    SECTION("Custom types") {
        struct SimpleStruct {
            int x;

            bool operator==(const SimpleStruct& other) const {
                return x == other.x;
            }
        };

        class SimpleClass {
        public:
            SimpleClass(int val) : value(val) {}

            [[nodiscard]]
            int getValue() const {
                return value;
            }

            bool operator==(const SimpleClass& other) const {
                return value == other.value;
            }

        private:
            int value;
        };

        // Value semantics
        REQUIRE_NOTHROW((rescpp::result<SimpleStruct, std::string>({42})));
        REQUIRE_NOTHROW((rescpp::result<SimpleClass, std::string>(SimpleClass(42))));

        // Check values
        rescpp::result<SimpleStruct, std::string> r1({ 42 });
        REQUIRE(r1.value().x == 42);

        rescpp::result<SimpleClass, std::string> r2(SimpleClass(42));
        REQUIRE(r2.value().getValue() == 42);
    }

    SECTION("Complex nested types") {
        // Nested containers
        using NestedVector = std::vector<std::vector<int>>;
        REQUIRE_NOTHROW((rescpp::result<NestedVector, std::string>(
            NestedVector{{1, 2}, {3, 4}})));

        // Complex map with container values
        using ComplexMap = std::map<std::string, std::vector<int>>;
        REQUIRE_NOTHROW((rescpp::result<ComplexMap, std::string>(
            ComplexMap{{"a", {1, 2}}, {"b", {3, 4}}})));

        // Tuple types
        using TupleType = std::tuple<int, std::string, double>;
        REQUIRE_NOTHROW((rescpp::result<TupleType, std::string>(
            TupleType{42, "hello", 3.14})));
    }

    SECTION("Move-only types") {
        // Unique pointer
        auto ptr = std::make_unique<int>(42);
        rescpp::result<std::unique_ptr<int>, std::string> r1(std::move(ptr));
        REQUIRE(*r1.value() == 42);
        REQUIRE(ptr == nullptr); // Ensure it was moved

        // Custom move-only type
        struct MoveOnly {
            std::unique_ptr<int> ptr;
            MoveOnly(int val) : ptr(std::make_unique<int>(val)) {}
            MoveOnly(MoveOnly&&) = default;
            MoveOnly& operator=(MoveOnly&&) = default;
            MoveOnly(const MoveOnly&) = delete;
            MoveOnly& operator=(const MoveOnly&) = delete;

            [[nodiscard]]
            int getValue() const {
                return *ptr;
            }
        };

        MoveOnly mo(42);
        rescpp::result<MoveOnly, std::string> r2(std::move(mo));
        REQUIRE(r2.value().getValue() == 42);
        REQUIRE(mo.ptr == nullptr); // Ensure it was moved
    }
}

// Test that various types can be used as error types in result<T, E>
TEST_CASE("Result compatibility with different error types", "[result][types]") {
    SECTION("Fundamental types as errors") {
        REQUIRE_NOTHROW((rescpp::result<std::string, int>(rescpp::detail::error, 42)));
        REQUIRE_NOTHROW((rescpp::result<std::string, unsigned int>(rescpp::detail::error, 42u)));
        REQUIRE_NOTHROW((rescpp::result<std::string, bool>(rescpp::detail::error, true)));
        REQUIRE_NOTHROW((rescpp::result<std::string, char>(rescpp::detail::error, 'A')));
    }

    SECTION("Standard library types as errors") {
        REQUIRE_NOTHROW((rescpp::result<int, std::string>(rescpp::detail::error, "error")));
        REQUIRE_NOTHROW((rescpp::result<int, std::vector<int>>(rescpp::detail::error, std::vector<int>{1, 2, 3})));

        // Enum types
        enum class ErrorCode { OK, ERROR, WARNING };
        REQUIRE_NOTHROW((rescpp::result<int, ErrorCode>(rescpp::detail::error, ErrorCode::ERROR)));
    }

    SECTION("Custom error types") {
        struct ErrorInfo {
            int code;
            std::string message;
            ErrorInfo(int c, std::string msg) : code(c), message(std::move(msg)) {}
        };

        REQUIRE_NOTHROW((rescpp::result<int, ErrorInfo>(rescpp::detail::error, ErrorInfo(500, "Server Error"))));

        // Check error values
        rescpp::result<int, ErrorInfo> r(rescpp::detail::error, ErrorInfo(404, "Not Found"));
        REQUIRE(r.has_error());
        REQUIRE(r.error().code == 404);
        REQUIRE(r.error().message == "Not Found");
    }

    SECTION("Complex error types") {
        // Nested error structures
        struct LocationInfo {
            std::string file;
            int line;
        };

        struct DetailedError {
            int code;
            std::string message;
            LocationInfo location;
            std::vector<std::string> trace;
        };

        DetailedError error{
            500,
            "Internal Server Error",
            { "main.cpp", 42 },
            { "function1", "function2", "main" }
        };

        REQUIRE_NOTHROW((rescpp::result<int, DetailedError>(rescpp::detail::error, std::move(error))));
    }
}

// Test fail function with various error types
TEST_CASE("Fail function with different error types", "[result][fail]") {
    SECTION("Fundamental error types") {
        auto f1 = rescpp::fail(42);
        rescpp::result<std::string, int> r1 = f1;
        REQUIRE(r1.has_error());
        REQUIRE(r1.error() == 42);

        auto f2 = rescpp::fail(true);
        rescpp::result<std::string, bool> r2 = f2;
        REQUIRE(r2.has_error());
        REQUIRE(r2.error() == true);
    }

    SECTION("Standard library error types") {
        auto f1 = rescpp::fail<std::string>("error message");
        rescpp::result<int, std::string> r1 = f1;
        REQUIRE(r1.has_error());
        REQUIRE(r1.error() == "error message");

        auto f2 = rescpp::fail(std::vector<int>{ 1, 2, 3 });
        rescpp::result<int, std::vector<int>> r2 = f2;
        REQUIRE(r2.has_error());
        REQUIRE(r2.error() == std::vector<int>{1, 2, 3});
    }

    SECTION("Custom error types") {
        struct ErrorInfo {
            int code;
            std::string message;

            ErrorInfo(int c, std::string msg) : code(c), message(std::move(msg)) {}

            bool operator==(const ErrorInfo& other) const {
                return code == other.code && message == other.message;
            }
        };

        // Using constructor
        auto f1 = rescpp::fail(ErrorInfo(404, "Not Found"));
        rescpp::result<int, ErrorInfo> r1 = f1;
        REQUIRE(r1.has_error());
        REQUIRE(r1.error() == ErrorInfo(404, "Not Found"));

        // Using std::in_place with constructor arguments
        auto f2 = rescpp::fail<ErrorInfo>(500, "Server Error");
        rescpp::result<int, ErrorInfo> r2 = f2;
        REQUIRE(r2.has_error());
        REQUIRE(r2.error() == ErrorInfo(500, "Server Error"));
    }
}

// Test void result with different error types
TEST_CASE("Void result with different error types", "[result][void]") {
    SECTION("Simple error types") {
        rescpp::result<void, int> r1;
        REQUIRE_FALSE(r1.has_error());

        rescpp::result<void, int> r2(rescpp::detail::error, 42);
        REQUIRE(r2.has_error());
        REQUIRE(r2.error() == 42);
    }

    SECTION("Complex error types") {
        struct ErrorInfo {
            int code;
            std::string message;

            ErrorInfo(int c, std::string msg) : code(c), message(std::move(msg)) {}

            bool operator==(const ErrorInfo& other) const {
                return code == other.code && message == other.message;
            }
        };

        rescpp::result<void, ErrorInfo> r1;
        REQUIRE_FALSE(r1.has_error());

        rescpp::result<void, ErrorInfo> r2(rescpp::detail::error, ErrorInfo(404, "Not Found"));
        REQUIRE(r2.has_error());
        REQUIRE(r2.error() == ErrorInfo(404, "Not Found"));
    }
}
