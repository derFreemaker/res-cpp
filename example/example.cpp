#include <res-cpp/res-cpp.hpp>

enum class some_error {
    none,
    test,
};

struct MoveOnlyStruct {
    explicit constexpr MoveOnlyStruct(int) noexcept {}

    MoveOnlyStruct() = delete;
    // MoveOnlyStruct(const MoveOnlyStruct&) = delete;
    // MoveOnlyStruct& operator=(const MoveOnlyStruct&) = delete;
};

template <>
struct rescpp::type_converter<MoveOnlyStruct, some_error> {
    static constexpr some_error convert(const MoveOnlyStruct& from) noexcept {
        return some_error::test;
    }
};

constexpr auto test_failure = rescpp::fail(MoveOnlyStruct{ 123 });

static constexpr rescpp::result<const int, MoveOnlyStruct> test_foo(bool fail) {
    if (fail) {
        // return 0;
        return test_failure;
    }

    return 123;
}

static constexpr rescpp::result<const bool, some_error> test_foo_chain(bool fail) {
    RESCPP_TRY_(foo, test_foo(fail));
    return foo != 0;
}

int main() {
    constexpr auto value = test_foo(false);
    constexpr auto value2 = test_foo(false);
    printf("%i %i\n", value.value(), value2.value());
    
    try {
        constexpr auto value3 = test_foo_chain(true);
        printf("%i\n", value3.value()); // also throws rescpp::detail::bad_result_access_exception when called, but crashed with abort
    }
    catch (const std::exception& e) {
        printf("%s\n", e.what());
    }
}
