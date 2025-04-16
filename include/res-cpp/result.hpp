#ifndef RESCPP_RESULT_H
#define RESCPP_RESULT_H

#include "res-cpp/reference_wrapper.hpp"
#include "res-cpp/result_base.hpp"
#include "res-cpp/type_traits.hpp"
#include "res-cpp/result_error.hpp"
#include "res-cpp/result_storage.hpp"

namespace ResCpp {
template <typename ResultT>
struct ResultHolder;

/// This is a none holdable type in other words.
/// 'Result<...>' type itself is just for type information.
/// Since everything is stored in static thread_local variables.
/// In order to keep the Result information you have to call '.hold()'.
/// This will copy the result data from the static thread_local variables to a
/// 'ResultHolder<...>' type struct.
template <typename StoredT, typename ErrorT = ResultError>
struct Result : detail::ResultBase<ErrorT> {
    using ErrorT = ErrorT;

    using StoredT = StoredT;
    using StoringT = std::conditional_t<std::is_reference_v<StoredT>,
                                        ReferenceWrapper<StoredT>,
                                        StoredT>;
    using ReturnT = std::conditional_t<std::is_reference_v<StoredT>,
                                       StoredT,
                                       make_lvalue_reference_t<StoredT>>;

    Result(detail::ErrorTag, ErrorT&& error) noexcept :
        detail::ResultBase<ErrorT>(detail::Error, std::forward<ErrorT>(error)) {}

    Result(detail::ErrorTag, const ErrorT& error) noexcept :
        detail::ResultBase<ErrorT>(detail::Error, error) {};

    template <typename... Args, class = std::enable_if_t<std::is_constructible_v<ErrorT, Args...>>>
    Result(detail::ErrorTag, Args&&... args) noexcept :
        Result(detail::Error, ErrorT(std::forward<Args>(args)...)) {}

    Result(detail::PassErrorTag<ErrorT>) noexcept :
        detail::ResultBase<ErrorT>(detail::PassErrorTag<ErrorT>{}) {}

    Result(StoredT&& value) noexcept :
        detail::ResultBase<ErrorT>(detail::Ok) {
        if constexpr (std::is_reference_v<StoredT>) {
            new(&ResultStorage<StoringT>()) StoringT(static_cast<StoredT>(value));
        }
        else {
            new(&ResultStorage<StoringT>()) StoredT(std::forward<StoredT>(value));
        }
    }

    template <typename T2>
        requires (!std::is_same_v<T2, StoredT> && std::is_nothrow_convertible_v<T2, StoredT>)
    Result(T2&& value) noexcept :
        detail::ResultBase<ErrorT>(detail::Ok) {
        new(&ResultStorage<StoringT>()) StoringT(static_cast<StoredT>(std::forward<T2>(value)));
    }

    ReturnT value() & {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<StoredT>) {
            return ResultStorage<StoringT>().get();
        }
        else {
            return ResultStorage<StoringT>();
        }
    }

    make_const_t<ReturnT> value() const & {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<StoredT>) {
            return ResultStorage<StoringT>().get();
        }
        else {
            return ResultStorage<StoringT>();
        }
    }

    ReturnT value() && {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<StoredT>) {
            return ResultStorage<StoringT>().get();
        }
        else {
            return ResultStorage<StoringT>();
        }
    }

    make_const_t<ReturnT> value() const && {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<StoredT>) {
            return ResultStorage<StoringT>().get();
        }
        else {
            return ResultStorage<StoringT>();
        }
    }

    ResultHolder<Result> hold() const noexcept {
        return ResultHolder<Result>(*this);
    }
};

template <typename ErrorT>
struct Result<void, ErrorT> : detail::ResultBase<ErrorT> {
    using ErrorT = ErrorT;

    Result(detail::ErrorTag, ErrorT&& error) noexcept :
        detail::ResultBase<ErrorT>(detail::Error, std::forward<ErrorT>(error)) {}

    Result(detail::ErrorTag, const ErrorT& error) noexcept :
        detail::ResultBase<ErrorT>(detail::Error, error) {};

    template <typename... Args>
    Result(detail::ErrorTag, Args&&... args) noexcept
        requires (std::is_constructible_v<ErrorT, Args...>) :
        Result(detail::Error, ErrorT(std::forward<Args>(args)...)) {}

    Result(detail::PassErrorTag<ErrorT>) noexcept :
        detail::ResultBase<ErrorT>(detail::PassErrorTag<ErrorT>{}) {}

    Result() noexcept :
        detail::ResultBase<ErrorT>(detail::Ok) {
        ResultErrorStorage<ErrorT>() = std::nullopt;
    }

    ResultHolder<Result> hold() const noexcept {
        return ResultHolder<Result>(*this);
    }
};
}

#endif //RESCPP_RESULT_H
