#ifndef RESCPP_TRY_H
#define RESCPP_TRY_H

#include "res-cpp/result.hpp"

namespace ResCpp::detail {
template <typename ErrorT>
void TryHelper(const Result<void, ErrorT>&) noexcept {}

template <typename T, typename ErrorT>
[[nodiscard]]
typename Result<T, ErrorT>::ReturnT TryHelper(Result<T, ErrorT> result) noexcept {
    return std::move(result).value();
}
}

#define RESULT_ERROR() \
    ::ResCpp::detail::Error

#define TRY_IMPL(expr, ...) \
    (::ResCpp::detail::TryHelper( \
        ({ \
            auto result_ = (expr); \
            if (result_.has_error()) { \
                __VA_ARGS__ \
            } \
            result_; \
        }) \
    ))

#define TRY(...) \
    TRY_IMPL((__VA_ARGS__), return result_;)

#endif //RESCPP_TRY_H
