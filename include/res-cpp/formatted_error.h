#ifndef RESCPP_FORMATTED_ERROR_H
#define RESCPP_FORMATTED_ERROR_H

#include <sstream>
#include <exception>
#include <fmt/format.h>

#include "res-cpp/result_error_base.h"

namespace ResCpp {
struct FormattedError : ResultErrorBase<FormattedError> {
private:
    std::string msg_;

public:
    explicit FormattedError(std::string msg) noexcept
        : msg_(std::move(msg)) {}

    template <typename... Args>
    explicit FormattedError(const std::string_view& fmt, Args&&... args) {
        try {
            msg_ = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
        }
        catch (const std::exception& e) {
            msg_ = std::string("exception: ") + e.what();
        }
    }
    
    void print(std::ostream& stream) const noexcept override {
        try {
            stream << msg_;
        }
        catch (const std::exception& _) {
            // we silently don't do anything
#ifndef NDEBUG
            __debugbreak();
#endif
        }
    }

    [[nodiscard]]
    std::string str() const noexcept override {
        try {
            std::stringstream stream;
            print(stream);
            return stream.str();
        }
        catch (const std::exception& _) {
            // we silently don't do anything
#ifndef NDEBUG
            __debugbreak();
#endif
        }

        return "";
    }
};
}

#endif //RESCPP_FORMATTED_ERROR_H
