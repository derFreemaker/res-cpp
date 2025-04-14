#ifndef RESCPP_RESULT_ERROR_BASE_H
#define RESCPP_RESULT_ERROR_BASE_H

#include <ostream>
#include <string>

#include "res-cpp/tags.h"

namespace ResCpp {
template <typename T, typename ErrorT>
struct Result;

template <typename DerivedT>
struct ResultErrorBase {
    virtual ~ResultErrorBase() = default;

    virtual void print(std::ostream& stream) const noexcept = 0;

    [[nodiscard]]
    virtual std::string str() const noexcept = 0;

    operator std::string() const noexcept {
        return str();
    }

    friend std::ostream& operator<<(std::ostream& stream, const DerivedT& error) {
        error.print(stream);
        return stream;
    }

    template <typename T>
    operator Result<T, DerivedT>() const noexcept {
        return Result<T, DerivedT>(detail::ErrorTag{}, *this);
    }
};
}

#endif //RESCPP_RESULT_ERROR_BASE_H
