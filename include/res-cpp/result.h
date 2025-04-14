#ifndef RESCPP_RESULT_H
#define RESCPP_RESULT_H

#include "res-cpp/reference_wrapper.h"
#include "res-cpp/result_base.h"
#include "res-cpp/type_traits.h"
#include "res-cpp/result_error.h"
#include "res-cpp/result_storage.h"

namespace ResCpp {
namespace detail {
template <typename ErrorT>
void ThrowBadValueAccessException() {
    throw std::runtime_error("Attempted to access value of an error Result. Error: " + ResultErrorStorage<ErrorT>().value().str());
}
}

template <typename T, typename ErrorT = ResultError>
struct Result : detail::ResultBase<ErrorT> {
    using StoringT = std::conditional_t<std::is_reference_v<T>,
                                        ReferenceWrapper<T>,
                                        T>;
    using ReturnT = std::conditional_t<std::is_reference_v<T>,
                                       T,
                                       make_lvalue_reference_t<T>>;

    Result(detail::ErrorTag, ErrorT&& error) noexcept
        : detail::ResultBase<ErrorT>(detail::Error, std::forward<ErrorT>(error)) {}

    Result(detail::ErrorTag, const ErrorT& error) noexcept
        : detail::ResultBase<ErrorT>(detail::Error, error) {};

    template <typename... Args, class = std::enable_if_t<std::is_constructible_v<ErrorT, Args...>>>
    Result(detail::ErrorTag, Args&&... args) noexcept
        : Result(detail::Error, ErrorT(std::forward<Args>(args)...)) {}

    Result(detail::PassErrorTag<ErrorT>) noexcept
        : detail::ResultBase<ErrorT>(detail::PassErrorTag<ErrorT>{}) {}

    Result(T&& value) noexcept : detail::ResultBase<ErrorT>(detail::Ok) {
        new(&ResultStorage<StoringT>()) StoringT(static_cast<T>(value));
    }

    template <typename T2>
        requires (!std::is_same_v<T2, T> && std::is_nothrow_convertible_v<T2, T>)
    Result(T2&& value) noexcept : detail::ResultBase<ErrorT>(detail::Ok) {
        new(&ResultStorage<StoringT>()) StoringT(static_cast<T>(std::forward<T2>(value)));
    }

    ReturnT Value() & {
        if (this->HasError()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<T>) {
            return ResultStorage<StoringT>().get();
        }
        else {
            return ResultStorage<StoringT>();
        }
    }

    make_const_t<ReturnT> Value() const & {
        if (this->HasError()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<T>) {
            return ResultStorage<StoringT>().get();
        }
        else {
            return ResultStorage<StoringT>();
        }
    }

    ReturnT Value() && {
        if (this->HasError()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<T>) {
            return ResultStorage<StoringT>().get();
        }
        else {
            return ResultStorage<StoringT>();
        }
    }

    make_const_t<ReturnT> Value() const && {
        if (this->HasError()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<T>) {
            return ResultStorage<StoringT>().get();
        }
        else {
            return ResultStorage<StoringT>();
        }
    }
};

template <typename ErrorT>
struct Result<void, ErrorT> : detail::ResultBase<ErrorT> {
    Result(detail::ErrorTag, ErrorT&& error) noexcept
        : detail::ResultBase<ErrorT>(detail::Error, std::forward<ErrorT>(error)) {}

    Result(detail::ErrorTag, const ErrorT& error) noexcept
        : detail::ResultBase<ErrorT>(detail::Error, error) {};

    template <typename... Args, class = std::enable_if_t<std::is_constructible_v<ErrorT, Args...>>>
    Result(detail::ErrorTag, Args&&... args) noexcept
        : Result(detail::Error, ErrorT(std::forward<Args>(args)...)) {}

    Result(detail::PassErrorTag<ErrorT>) noexcept
        : detail::ResultBase<ErrorT>(detail::PassErrorTag<ErrorT>{}) {}
    
    Result() noexcept : detail::ResultBase<ErrorT>(detail::Ok) {
        ResultErrorStorage<ErrorT>() = std::nullopt;
    }
};
}

#endif //RESCPP_RESULT_H
