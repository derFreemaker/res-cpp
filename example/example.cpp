#include <res-cpp/res-cpp.hpp>

struct some_error {
    int number;

    inline constexpr some_error(const int number) : number(number) {}
};

struct some_other_error {
    int number;

    inline constexpr some_other_error(const int number) : number(number) {}

    inline constexpr operator some_error() const noexcept {
        return { number };
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

static constexpr rescpp::result<bool, some_error> test_foo_chain(bool fail) {
    TRY_(foo, test_foo(fail));
    
    return foo != 0;
}

int main() {
    constexpr auto value = test_foo(false);
    constexpr auto value2 = test_foo(false);
    printf("%i %i\n", value.value(), value2.value());

    constexpr auto value3 = test_foo_chain(false);
    printf("%i\n", value3.value());
}
