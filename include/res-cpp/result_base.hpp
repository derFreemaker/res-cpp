#ifndef RESCPP_RESULT_BASE_H
#define RESCPP_RESULT_BASE_H

#include "res-cpp/exceptions.hpp"
#include "res-cpp/tags.hpp"
#include "res-cpp/result_error.hpp"
#include "res-cpp/result_storage.hpp"

namespace ResCpp::detail {
template <typename ErrorT = FormattedError>
    requires (!std::is_reference_v<ErrorT> && !std::is_pointer_v<ErrorT>) //TODO: improve with concept for better errors
struct ResultBase {
    ResultBase(OkTag) noexcept {
        ResultErrorStorage<ErrorT>() = std::nullopt;
    }

    ResultBase(ErrorTag, ErrorT&& error) noexcept {
        ResultErrorStorage<ErrorT>() = std::forward<ErrorT>(error);
    }

    ResultBase(ErrorTag, const ErrorT& error) noexcept {
        ResultErrorStorage<ErrorT>() = std::forward<ErrorT>(error);
    }

    ResultBase(PassErrorTag<ErrorT>) noexcept {}

    [[nodiscard]]
    bool has_error() const noexcept {
        return ResultErrorStorage<ErrorT>().has_value();
    }

    [[nodiscard]]
    const ErrorT& error() const {
        if (!has_error()) {
            ThrowBadErrorAccessException();
        }
        return ResultErrorStorage<ErrorT>().value();
    }
};
}

#endif //RESCPP_RESULT_BASE_H
