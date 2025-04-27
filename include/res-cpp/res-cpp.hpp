#ifndef RESCPP_H
#define RESCPP_H

#include <stdexcept>
#include <optional>
#include <type_traits>
#include <memory>

namespace rescpp {
namespace detail {
template <typename T>
struct make_const {
    using type = const T;
};

template <typename T>
struct make_const<const T> {
    using type = const T;
};

template <typename T>
using make_const_t = typename make_const<T>::type;

template <typename T>
using make_lvalue_reference_t = std::remove_reference_t<T>&;
}

template <typename From, typename To>
struct type_converter;

namespace detail {
template <typename From, typename To>
concept has_type_converter = requires(const From& from) {
    { type_converter<From, To>::convert(from) } noexcept -> std::same_as<To>;
};

struct error_tag {};

inline constexpr error_tag error{};

struct pass_error_tag {};

inline constexpr pass_error_tag pass_error{};

struct bad_result_access_exception final : std::logic_error {
    explicit bad_result_access_exception(const char* msg)
        : std::logic_error(msg) {}
};

#if defined(NDEBUG)
#define RESCPP_NOEXCEPT noexcept
#else
#define RESCPP_NOEXCEPT
#endif

inline void throw_bad_error_access_exception() {
    const char* bad_error_access_exception_message = "cannot access error on a good result";
#if defined(RESCPP_DISABLE_EXCEPTIONS)
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
    const char* bad_value_access_exception_message = "cannot access value on a bad result";
#if defined(RESCPP_DISABLE_EXCEPTIONS)
    std::fprintf(
        stderr,
        bad_value_access_exception_message
    );
    std::abort();
#else
    throw bad_result_access_exception(bad_value_access_exception_message);
#endif
}

template <typename>
struct reference_wrapper;

template <typename T>
struct reference_wrapper<T&> {
private:
    T* ptr;

public:
    inline constexpr reference_wrapper(T& ref) noexcept
        : ptr(std::addressof(ref)) {}

    inline constexpr ~reference_wrapper() noexcept = default;
    
    inline constexpr T& get() const noexcept {
        return *ptr;
    }

    inline constexpr T* operator->() const noexcept {
        return ptr;
    }

    inline constexpr T& operator*() const noexcept {
        return *ptr;
    }
};

template <typename T>
struct reference_wrapper<const T&> {
private:
    const T* ptr;

public:
    inline constexpr reference_wrapper(const T& ref) noexcept
        : ptr(std::addressof(ref)) {}

    inline constexpr ~reference_wrapper() noexcept = default;
    
    inline constexpr const T& get() const noexcept {
        return *ptr;
    }

    inline constexpr const T* operator->() const noexcept {
        return ptr;
    }

    inline constexpr operator const T&() const noexcept {
        return *ptr;
    }
};
}

template <typename>
struct failure;

template <typename T, typename E>
struct result {
    using value_type = T;

    using error_type = E;
    static_assert((!std::is_reference_v<error_type>
                      && !std::is_pointer_v<error_type>),
                  "can not use references or pointer as error type");

    template <typename V>
    using return_value_type = std::conditional_t<(
                                                     std::is_reference_v<value_type>
                                                 ),
                                                 std::conditional_t<std::is_const_v<V>,
                                                                    detail::make_const_t<value_type>,
                                                                    value_type>,
                                                 V>;

private:
    using storing_type = std::conditional_t<std::is_lvalue_reference_v<value_type>,
                                            detail::reference_wrapper<std::remove_volatile_t<value_type>>,
                                            std::remove_const_t<value_type>>;

    union {
        error_type error_;
        storing_type value_;
    };

    bool has_error_;

public:
    inline constexpr result(detail::error_tag, const error_type&& error) noexcept
        : error_(std::forward<const error_type>(error)), has_error_(true) {}

    inline constexpr result(value_type&& value) noexcept
        : value_(std::forward<value_type>(value)), has_error_(false) {}

    template <typename... Args>
    explicit inline constexpr result(std::in_place_t, Args&&... args) noexcept
        : value_(std::forward<Args>(args)...), has_error_(false) {}

    template <typename T2>
        requires (!std::is_same_v<value_type, T2>
            && std::is_convertible_v<T2, value_type>)
    inline constexpr result(T2&& value)
        noexcept(std::is_nothrow_convertible_v<T2, value_type>)
        : value_(static_cast<value_type>(std::forward<T2>(value))), has_error_(false) {}

    inline constexpr result(const result& other)
        noexcept(std::is_nothrow_copy_constructible_v<value_type>
            && std::is_nothrow_copy_constructible_v<error_type>)
        requires (std::is_copy_constructible_v<value_type>
            && std::is_copy_constructible_v<error_type>)
        : has_error_(other.has_error_) {
        if (has_error_) {
            std::construct_at(std::addressof(error_), other.error_);
        }
        else {
            std::construct_at(std::addressof(value_), other.value_);
        }
    }

    inline constexpr result(result&& other)
        noexcept(std::is_nothrow_move_constructible_v<storing_type>
            && std::is_nothrow_move_constructible_v<error_type>)
        requires (std::is_move_constructible_v<storing_type>
            && std::is_move_constructible_v<error_type>)
        : has_error_(other.has_error_) {
        if (has_error_) {
            std::construct_at(std::addressof(error_), std::move(other.error_));
        }
        else {
            std::construct_at(std::addressof(value_), std::move(other.value_));
        }
    }

    inline constexpr ~result() noexcept {
        if (has_error_ && std::is_destructible_v<error_type>) {
            std::destroy_at(std::addressof(error_));
            return;
        }

        if constexpr (std::is_destructible_v<storing_type> && !std::is_trivially_destructible_v<storing_type>) {
            std::destroy_at(std::addressof(value_));
        }
    }

    [[nodiscard]]
    inline constexpr bool has_error() const noexcept {
        return has_error_;
    }

    [[nodiscard]]
    inline constexpr const error_type& error() const & RESCPP_NOEXCEPT {
#ifndef NDEBUG
        if (!has_error()) {
            detail::throw_bad_error_access_exception();
        }
#endif

        return error_;
    }

    [[nodiscard]]
    inline constexpr const error_type&& error() const && RESCPP_NOEXCEPT {
#ifndef NDEBUG
        if (!has_error()) {
            detail::throw_bad_error_access_exception();
        }
#endif

        return std::move(error_);
    }

    [[nodiscard]]
    inline constexpr return_value_type<value_type&> value() & RESCPP_NOEXCEPT {
#ifndef NDEBUG
        if (has_error()) {
            detail::throw_bad_value_access_exception();
        }
#endif

        if constexpr (std::is_lvalue_reference_v<value_type>) {
            return value_.get();
        }
        else if constexpr (std::is_rvalue_reference_v<value_type>) {
            return std::move(value_);
        }
        else {
            return value_;
        }
    }

    [[nodiscard]]
    inline constexpr return_value_type<const value_type&> value() const & RESCPP_NOEXCEPT {
#ifndef NDEBUG
        if (has_error()) {
            detail::throw_bad_value_access_exception();
        }
#endif

        if constexpr (std::is_lvalue_reference_v<value_type>) {
            return value_.get();
        }
        else if constexpr (std::is_rvalue_reference_v<value_type>) {
            return std::move(value_);
        }
        else {
            return value_;
        }
    }

    [[nodiscard]]
    inline constexpr return_value_type<value_type&&> value() && RESCPP_NOEXCEPT {
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
    inline constexpr return_value_type<const value_type&&> value() const && RESCPP_NOEXCEPT {
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

    // inline constexpr operator return_value_type<value_type&>() & noexcept {
    //     return std::forward<return_value_type<value_type>>(value());
    // }
    //
    // inline constexpr operator return_value_type<const value_type&>() const & noexcept {
    //     return std::forward<return_value_type<const value_type>>(value());
    // }
    //
    // inline constexpr operator return_value_type<value_type&&>() && noexcept {
    //     return std::forward<return_value_type<value_type>>(value());
    // }
    //
    // inline constexpr operator return_value_type<const value_type&&>() const && noexcept {
    //     return std::forward<return_value_type<const value_type>>(value());
    // }

    inline constexpr return_value_type<value_type&> operator*() & RESCPP_NOEXCEPT {
        return value();
    }

    inline constexpr return_value_type<const value_type&> operator*() const & RESCPP_NOEXCEPT {
        return value();
    }

    inline constexpr return_value_type<value_type&&> operator*() && RESCPP_NOEXCEPT {
        return std::forward<return_value_type<value_type>>(value());
    }

    inline constexpr return_value_type<const value_type&&> operator*() const && RESCPP_NOEXCEPT {
        return std::forward<return_value_type<const value_type>>(value());
    }

    template <typename T2, typename E2>
    inline constexpr operator result<T2, E2>() const & noexcept(std::is_nothrow_convertible_v<value_type, T2>
        && std::is_nothrow_convertible_v<error_type, E2>) {
        if (has_error()) {
            return failure<error_type>(error_);
        }
        return result<T2, E2>(value_);
    }

    template <typename T2, typename E2>
    inline constexpr operator result<T2, E2>() const && noexcept(std::is_nothrow_convertible_v<value_type, T2>
        && std::is_nothrow_convertible_v<error_type, E2>) {
        if (has_error()) {
            return failure<error_type>(error_);
        }
        return result<T2, E2>(std::move(value_));
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
    const std::optional<error_type> error_;

public:
    inline constexpr result(detail::error_tag, const error_type&& error) noexcept
        : error_(std::forward<const error_type>(error)) {}

    inline constexpr result() noexcept
        : error_(std::nullopt) {}

    inline constexpr result(const result& other)
        noexcept(std::is_nothrow_copy_constructible_v<error_type>)
        requires (std::is_copy_constructible_v<error_type>)
        : error_(other.error_) {}

    inline constexpr result(result&& other)
        noexcept(std::is_nothrow_move_constructible_v<error_type>)
        requires (std::is_move_constructible_v<error_type>)
        : error_(std::move(other.error_)) {}

    [[nodiscard]]
    inline constexpr bool has_error() const noexcept {
        return error_.has_value();
    }

    [[nodiscard]]
    inline constexpr const error_type& error() const & RESCPP_NOEXCEPT {
#ifndef NDEBUG
        if (!has_error()) {
            detail::throw_bad_error_access_exception();
        }
#endif

        return error_.value();
    }

    [[nodiscard]]
    inline constexpr const error_type&& error() const && RESCPP_NOEXCEPT {
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
    const error_type error_;

public:
    explicit inline constexpr failure(const E& error) noexcept
        : error_(std::forward<const E>(error)) {}

    explicit inline constexpr failure(const E&& error) noexcept
        : error_(std::forward<const E>(error)) {}

    template <typename... Args>
    inline constexpr failure(std::in_place_t, Args&&... args) noexcept
        : error_(std::forward<Args>(args)...) {}

    [[nodiscard]]
    inline constexpr const error_type& error() const & noexcept {
        return error_;
    }

    [[nodiscard]]
    inline constexpr const error_type&& error() const && noexcept {
        return std::move(error_);
    }

    template <typename T>
    inline constexpr operator result<T, error_type>() const noexcept {
        return result<T, error_type>(detail::error, std::move(error_));
    }

    template <typename T, typename E2>
        requires (!std::is_same_v<error_type, E2> && std::is_constructible_v<E2, error_type>)
    inline constexpr operator result<T, E2>() const noexcept(std::is_nothrow_constructible_v<E2, error_type>) {
        return result<T, E2>(detail::error, static_cast<E2>(error_));
    }

    template <typename T, typename E2>
        requires (!std::is_same_v<error_type, E2> && !std::is_constructible_v<E2, error_type>
            && detail::has_type_converter<std::remove_cvref_t<error_type>, std::remove_cvref_t<E2>>)
    inline constexpr operator result<T, E2>() const noexcept {
        return result<T, E2>(detail::error,
                             type_converter<
                                 std::remove_cvref_t<error_type>,
                                 std::remove_cvref_t<E2>
                             >::convert(error_)
        );
    }
};

template <typename E>
inline constexpr failure<E> fail(const E&& error) noexcept {
    return failure<E>(std::forward<const E>(error));
}

template <typename E, typename... Args>
inline constexpr failure<E> fail(Args&&... args) noexcept {
    return failure<E>(std::in_place, std::forward<Args>(args)...);
}

template <typename E>
inline constexpr failure<E> fail(detail::pass_error_tag, const E&& error) noexcept {
    return failure<E>(std::forward<const E>(error));
}

namespace detail {
template <typename E>
inline constexpr void try_helper(result<void, E>&&) {}

template <typename E>
inline constexpr void try_helper(const result<void, E>&&) {}

template <typename T, typename E>
inline constexpr auto try_helper(result<T, E>&& res) {
    return std::forward<typename result<T, E>::template return_value_type<T&&>>(res.value());
}

template <typename T, typename E>
inline constexpr auto try_helper(const result<T, E>&& res) {
    return std::forward<typename result<T, E>::template return_value_type<const T&&>>(res.value());
}
}

#if !defined(RESCPP_DISABLE_TRY_MACRO)

/// WARNING: NOT 'constexpr' compatible
/// 'result_' is the used identifier for the result object.
#define RESCPP_TRY_IMPL(expr, ...) \
    ::rescpp::detail::try_helper(({ \
        auto&& result_ = (expr); \
        if (result_.has_error()) { \
            __VA_ARGS__ \
        } \
        std::move(result_); \
    }))

/// WARNING: NOT 'constexpr' compatible
#define RESCPP_TRY(...) \
    RESCPP_TRY_IMPL((__VA_ARGS__), \
        return ::rescpp::fail(::rescpp::detail::pass_error, \
            std::move(result_.error()) \
        ); \
    )

#define RESCPP_CONCAT_IMPL(a, b) a##b
#define RESCPP_CONCAT(a, b) RESCPP_CONCAT_IMPL(a, b)

#define RESCPP_TRY_RESULT_NAME(name) RESCPP_CONCAT(name, _result)
#define RESCPP_TRY_RESULT_TYPE(name) RESCPP_CONCAT(name, _result_type)

/// '<name>_result' is the used identifier for the result object.
/// It's recommended to use 'RESCPP_TRY_RESULT_NAME(<name>)' for result object.
/// And to use 'RESCPP_TRY_RESULT_TYPE(<name>)' for result type.
#define RESCPP_TRY_IMPL_(name, expr, ...) \
    auto RESCPP_TRY_RESULT_NAME(name) = (expr); \
    using RESCPP_TRY_RESULT_TYPE(name) = decltype(RESCPP_TRY_RESULT_NAME(name)); \
    if (RESCPP_TRY_RESULT_NAME(name).has_error()) { \
        __VA_ARGS__ \
    } \
    typename RESCPP_TRY_RESULT_TYPE(name)::value_type name = ::rescpp::detail::try_helper(std::move(RESCPP_TRY_RESULT_NAME(name)));

#define RESCPP_TRY_(name, ...) \
    RESCPP_TRY_IMPL_(name, (__VA_ARGS__), \
        return ::rescpp::fail(::rescpp::detail::pass_error, \
            std::move(RESCPP_TRY_RESULT_NAME(name)).error() \
        ); \
    )

#endif
}

#endif //RESCPP_H
