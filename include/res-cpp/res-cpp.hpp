#ifndef RESCPP_H
#define RESCPP_H

#include <stdexcept>
#include <optional>
#include <type_traits>

namespace rescpp {
namespace detail {
struct error_tag {};

inline constexpr error_tag error{};

struct bad_result_access_exception final : std::logic_error {
    explicit bad_result_access_exception(const char* msg)
        : std::logic_error(msg) {}
};

inline void throw_bad_error_access_exception() {
    constexpr const char* bad_error_access_exception_message = "cannot access error on a good result";
#if defined(RESCPP_RESULT_DISABLE_EXCEPTIONS)
    std::fprintf(
        stderr,
        bad_error_access_exception_message
    );
    std::abort();
#else
    throw bad_result_access_exception(bad_error_access_exception_message);
#endif
}

inline void throw_bad_value_access_exception() {
    constexpr const char* bad_value_access_exception_message = "cannot access value on a fail result";
#if defined(RESCPP_RESULT_DISABLE_EXCEPTIONS)
    std::fprintf(
        stderr,
        bad_value_access_exception_message
    );
    std::abort();
#else
    throw bad_result_access_exception(bad_value_access_exception_message);
#endif
}
}

template <typename T, typename E>
struct result {
    using value_type = T;

    using error_type = E;
    static_assert((!std::is_reference_v<error_type>
                  && !std::is_pointer_v<error_type>),
                  "can not use references or pointer as error type");

private:
    using storing_type = std::conditional_t<std::is_lvalue_reference_v<value_type>,
                                            std::reference_wrapper<std::remove_reference_t<value_type>>,
                                            value_type>;

    template <typename V>
    using return_value_type = std::conditional_t<(
                                                     std::is_reference_v<value_type>
                                                     || std::is_pointer_v<value_type>
                                                 ),
                                                 value_type,
                                                 V>;

    union {
        error_type error_;
        storing_type value_;
    };

    bool has_error_;

public:
    inline constexpr result(detail::error_tag, error_type&& error) noexcept
        : error_(std::forward<error_type>(error)), has_error_(true) {}

    template <typename E2>
        requires(!std::is_same_v<error_type, E2>
            && std::is_nothrow_convertible_v<E2, error_type>)
    inline constexpr result(detail::error_tag, E2&& error) noexcept
        : error_(std::forward<E2>(error)), has_error_(true) {}

    inline constexpr result(value_type&& value) noexcept
        : value_(std::forward<value_type>(value)), has_error_(false) {}

    template <typename T2>
        requires (!std::is_same_v<value_type, T2>
            && std::is_nothrow_convertible_v<T2, value_type>)
    inline constexpr result(T2&& value) noexcept
        : value_(std::forward<T2>(value)), has_error_(false) {}

    [[nodiscard]]
    inline constexpr bool has_error() const noexcept {
        return has_error_;
    }

    [[nodiscard]]
    inline constexpr const error_type& error() const & noexcept {
#ifndef NDEBUG
        if (!has_error()) {
            detail::throw_bad_error_access_exception();
        }
#endif

        return error_;
    }

    [[nodiscard]]
    inline constexpr error_type&& error() && noexcept {
#ifndef NDEBUG
        if (!has_error()) {
            detail::throw_bad_error_access_exception();
        }
#endif

        return std::move(error_);
    }
    
    [[nodiscard]]
    inline constexpr const error_type&& error() const && noexcept {
#ifndef NDEBUG
        if (!has_error()) {
            detail::throw_bad_error_access_exception();
        }
#endif

        return std::move(error_);
    }

    [[nodiscard]]
    inline constexpr return_value_type<value_type&> value() & noexcept {
#ifndef NDEBUG
        if (has_error()) {
            detail::throw_bad_value_access_exception();
        }
#endif

        if constexpr (std::is_lvalue_reference_v<value_type>) {
            return value_.get();
        }
        else {
            return value_;
        }
    }

    [[nodiscard]]
    inline constexpr return_value_type<const value_type&> value() const & noexcept {
#ifndef NDEBUG
        if (has_error()) {
            detail::throw_bad_value_access_exception();
        }
#endif

        if constexpr (std::is_lvalue_reference_v<value_type>) {
            return value_.get();
        }
        else {
            return value_;
        }
    }

    [[nodiscard]]
    inline constexpr return_value_type<value_type&&> value() && noexcept {
#ifndef NDEBUG
        if (has_error()) {
            detail::throw_bad_value_access_exception();
        }
#endif

        if constexpr (std::is_lvalue_reference_v<value_type>) {
            return value_.get();
        }
        else {
            return std::move(value_);
        }
    }

    [[nodiscard]]
    inline constexpr return_value_type<const value_type&&> value() const && noexcept {
#ifndef NDEBUG
        if (has_error()) {
            detail::throw_bad_value_access_exception();
        }
#endif

        if constexpr (std::is_lvalue_reference_v<value_type>) {
            return value_.get();
        }
        else {
            return std::move(value_);
        }
    }
};

template <typename E>
struct result<void, E> {
    using value_type = void;

    using error_type = E;
    static_assert((!std::is_reference_v<error_type>
                  && !std::is_pointer_v<error_type>),
                  "can not use references or pointer as error type");

private:
    std::optional<error_type> error_;

public:
    inline constexpr result(detail::error_tag, error_type&& error) noexcept
        : error_(std::forward<error_type>(error)) {}

    inline constexpr result() noexcept
        : error_(std::nullopt) {}

    [[nodiscard]]
    inline constexpr bool has_error() const noexcept {
        return error_.has_value();
    }

    [[nodiscard]]
    inline constexpr const error_type& error() const & noexcept {
#ifndef NDEBUG
        if (!has_error()) {
            detail::throw_bad_error_access_exception();
        }
#endif

        return error_.value();
    }

    [[nodiscard]]
    inline constexpr error_type&& error() const && noexcept {
#ifndef NDEBUG
        if (!has_error()) {
            detail::throw_bad_error_access_exception();
        }
#endif

        return std::move(error_).value();
    }
};

template <typename E>
struct failure {
    using error_type = E;
    static_assert((!std::is_reference_v<error_type>
                  && !std::is_pointer_v<error_type>),
                  "can not use references or pointer as error type");

private:
    error_type error_;

public:
    inline constexpr failure(E&& error) noexcept
        : error_(std::forward<E>(error)) {}

    [[nodiscard]]
    inline constexpr const error_type& error() const & noexcept {
        return error_;
    }

    [[nodiscard]]
    inline constexpr error_type&& error() && noexcept {
        return std::move(error_);
    }

    [[nodiscard]]
    inline constexpr const error_type&& error() const && noexcept {
        return std::move(error_);
    }

    template <typename T>
    inline constexpr operator result<T, E>() noexcept {
        return result<T, E>(detail::error, std::forward<E>(error_));
    }
};

template <typename E>
inline constexpr failure<E> fail(E&& error) noexcept {
    return failure<E>(std::forward<E>(error));
}
}

#endif //RESCPP_H
