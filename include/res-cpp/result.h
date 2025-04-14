#ifndef RESCPP_RESULT_H
#define RESCPP_RESULT_H

#include "res-cpp/type_traits.h"
#include "res-cpp/result_error.h"
#include "res-cpp/result_storage.h"

namespace ResCpp {
template <typename T, typename ErrorT = ResultError>
    requires (!std::is_reference_v<ErrorT> && !std::is_pointer_v<T>) //TODO: improve with concept for better errors
struct Result {
    using ReturnT = std::conditional_t<std::is_reference_v<T>,
                                       T,
                                       make_lvalue_reference_t<T>>;

    Result(detail::ErrorTag, ErrorT&& error) noexcept {
        ResultErrorStorage<ErrorT>() = std::forward<ErrorT>(error);
    }

    Result(detail::ErrorTag, const ErrorT& error) noexcept {
        ResultErrorStorage<ErrorT>() = std::forward<ErrorT>(error);
    }

    Result(T&& value) noexcept {
        std::memcpy(&ResultStorage<T>(), std::addressof(value), sizeof(T));
    }

    template <typename T2>
        requires (!std::is_same_v<T2, T> && std::is_nothrow_convertible_v<T2, T>)
    Result(T2&& value) noexcept {
        
    }
};
}

#endif //RESCPP_RESULT_H
