#ifndef RESCPP_RESULT_H
#define RESCPP_RESULT_H

#include <optional>

#include "res-cpp/reference_wrapper.hpp"
#include "res-cpp/exceptions.hpp"
#include "res-cpp/tags.hpp"
#include "res-cpp/type_traits.hpp"
#include "res-cpp/result_error.hpp"

namespace ResCpp {

template <typename StoredT, typename ErrorT = ResultError>
struct Result {
    using ErrorT = ErrorT;

    using StoredT = StoredT;
    using StoringT = std::conditional_t<std::is_lvalue_reference_v<StoredT>,
                                        ReferenceWrapper<StoredT>,
                                        std::remove_reference_t<StoredT>>;
    using ReturnT = std::conditional_t<std::is_reference_v<StoredT>,
                                       StoredT,
                                       make_lvalue_reference_t<StoredT>>;

private:
    union {
        ErrorT error_;
        StoringT value_;
    };

    bool has_error_;

public:
    Result(detail::ErrorTag, ErrorT&& error) noexcept
        : error_(std::forward<ErrorT>(error)), has_error_(true) {}

    Result(detail::ErrorTag, const ErrorT& error) noexcept
        : error_(error), has_error_(true) {}

    template <typename... Args>
    Result(detail::ErrorTag, Args&&... args) noexcept
        requires (std::is_constructible_v<ErrorT, Args...>)
        : Result(detail::Error, ErrorT(std::forward<Args>(args)...)) {}

    Result(StoredT&& value) noexcept
        requires (std::is_lvalue_reference_v<StoredT>)
        : value_(ReferenceWrapper<StoredT>(std::forward<StoredT>(value))), has_error_(false) {}

    Result(StoredT&& value) noexcept
        requires (!std::is_lvalue_reference_v<StoredT>)
        : value_(std::forward<StoredT>(value)), has_error_(false) {}

    template <typename T2>
        requires (!std::is_same_v<T2, StoredT> && std::is_nothrow_convertible_v<T2, StoredT>)
    Result(T2&& value) noexcept
        : Result(static_cast<StoredT>(std::forward<T2>(value))) {}

    Result(const Result& other) noexcept {
        has_error_ = other.has_error_;
        if (has_error_) {
            error_ = other.error_;
        }
        else {
            value_ = other.value_;
        }
    }

    ~Result() noexcept {}

    [[nodiscard]]
    bool has_error() const noexcept {
        return has_error_;
    }

    [[nodiscard]]
    const ErrorT& error() const {
        if (!has_error()) {
            detail::ThrowBadErrorAccessException();
        }
        return error_;
    }

    ReturnT value() & {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException(error_);
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return value_.get();
        }
        else {
            return value_;
        }
    }

    make_const_t<ReturnT> value() const & {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException(error_);
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return value_.get();
        }
        else {
            return value_;
        }
    }

    ReturnT value() && {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException(error_);
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return value_.get();
        }
        else {
            return std::move(value_);
        }
    }

    make_const_t<ReturnT> value() const && {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException(error_);
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return value_.get();
        }
        else {
            return std::move(value_);
        }
    }

    template <typename OtherStoredT, typename OtherErrorT>
        requires (std::is_nothrow_convertible_v<ErrorT, OtherErrorT>)
    operator Result<OtherStoredT, OtherErrorT>() noexcept {
        return Result<OtherStoredT, OtherErrorT>(detail::Error, error());
    }
};

template <typename ErrorT>
struct Result<void, ErrorT> {
    using ErrorT = ErrorT;

private:
    std::optional<ErrorT> error_;

public:
    Result(detail::ErrorTag, ErrorT&& error) noexcept
        : error_(std::forward<ErrorT>(error)) {}

    Result(detail::ErrorTag, const ErrorT& error) noexcept
        : error_(error) {};

    template <typename... Args>
    Result(detail::ErrorTag, Args&&... args) noexcept
        requires (std::is_constructible_v<ErrorT, Args...>)
        : Result(detail::Error, ErrorT(std::forward<Args>(args)...)) {}

    Result() noexcept
        : error_(std::nullopt) {}

    Result(const Result& other) noexcept {
        error_ = other.error_;
    }

    ~Result() noexcept = default;

    [[nodiscard]]
    bool has_error() const noexcept {
        return error_.has_value();
    }

    [[nodiscard]]
    const ErrorT& error() const {
        if (!has_error()) {
            detail::ThrowBadErrorAccessException();
        }
        return error_.value();
    }

    template <typename OtherStoredT, typename OtherErrorT>
        requires (std::is_nothrow_convertible_v<ErrorT, OtherErrorT>)
    operator Result<OtherStoredT, OtherErrorT>() noexcept {
        return Result<OtherStoredT, OtherErrorT>(detail::Error, error());
    }
};
}

#endif //RESCPP_RESULT_H
