#ifndef RESULT_EXCEPTIONS_HPP
#define RESULT_EXCEPTIONS_HPP

#include <stdexcept>

namespace ResCpp::detail {
template <typename ErrorT>
void ThrowBadValueAccessException(const ErrorT& error) {
    throw std::runtime_error("Attempted to access value of an error Result. Error: " + error.str());
}

inline void ThrowBadErrorAccessException() {
    throw std::logic_error("Attempted to access error of an success Result.");
}
}

#endif //RESULT_EXCEPTIONS_HPP
