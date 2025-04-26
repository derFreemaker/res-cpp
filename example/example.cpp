#include <res-cpp/res-cpp.hpp>

enum class some_error {
    none,
    test,
};

enum class some_other_error {
    none,
    test2
};

template <>
struct rescpp::type_converter<some_other_error, some_error> {
    static constexpr some_error convert(const some_other_error&& from) noexcept {
        return static_cast<some_error>(from);
    }
};

constexpr auto test_failure = rescpp::fail(some_other_error{ 123 });

static constexpr rescpp::result<const int, some_other_error> test_foo(bool fail) {
    if (fail) {
        // return 0;
        return test_failure;
    }

    return 123;
}

static constexpr rescpp::result<const bool, some_error> test_foo_chain(bool fail) {
    // RESCPP_TRY_(foo, );
    return *test_foo(fail) != 0;
}

int main() {
    constexpr auto value = test_foo(false);
    constexpr auto value2 = test_foo(false);
    printf("%i %i\n", value.value(), value2.value());

    constexpr auto value3 = test_foo_chain(false);
    printf("%i\n", value3.value());
}
