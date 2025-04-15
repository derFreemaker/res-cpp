#ifndef RESCPP_TRY_H
#define RESCPP_TRY_H

#include "res-cpp/result.h"

namespace ResCpp::detail {
template <typename ErrorT>
void TryHelper(Result<void, ErrorT>*) noexcept {}

template <typename ErrorT>
void TryHelper(const Result<void, ErrorT>*) noexcept {}

template <typename T, typename ErrorT>
[[nodiscard]]
typename Result<T, ErrorT>::ReturnT TryHelper(Result<T, ErrorT>*) noexcept {
    if constexpr (std::is_reference_v<T>) {
        return ResultStorage<typename Result<T, ErrorT>::StoringT>().get();
    }
    else {
        return ResultStorage<typename Result<T, ErrorT>::StoringT>();
    }
}

template <typename T, typename ErrorT>
[[nodiscard]]
typename Result<T, ErrorT>::ReturnT TryHelper(const Result<T, ErrorT>*) noexcept {
    if constexpr (std::is_reference_v<T>) {
        return ResultStorage<typename Result<T, ErrorT>::StoringT>().get();
    }
    else {
        return ResultStorage<typename Result<T, ErrorT>::StoringT>();
    }
}

struct PassErrorHelper {
    template <typename T, typename ErrorT>
    operator Result<T, ErrorT>() const noexcept {
        return Result<T, ErrorT>(PassErrorTag<ErrorT>{});
    }
};
}

#define RESULT_ERROR() \
    ::ResCpp::detail::Error

#define TRY_IMPL(expr, ...) \
    (::ResCpp::detail::TryHelper( \
        ({ \
            auto __result__ = (expr); \
            if (__result__.has_error()) { \
                __VA_ARGS__ \
            } \
            &__result__; \
        }) \
    ))

#define TRY(...) \
    TRY_IMPL((__VA_ARGS__), return ::ResCpp::detail::PassErrorHelper{};)

#endif //RESCPP_TRY_H
