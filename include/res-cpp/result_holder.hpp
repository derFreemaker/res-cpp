#ifndef RESCPP_RESULT_HOLDER_HPP
#define RESCPP_RESULT_HOLDER_HPP

#include "res-cpp/result.hpp"

namespace ResCpp {

template <typename StoredT, typename ErrorT>
struct ResultHolder {
    using StoredT = StoredT;
    using ErrorT = ErrorT;

    using StoringT = std::conditional_t<std::is_reference_v<StoredT>,
                                        ReferenceWrapper<StoredT>,
                                        StoredT>;
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
    ResultHolder(StoringT&& value) :
        value_(std::forward<StoringT>(value)),
        has_error_(false) {}

    ResultHolder(ErrorT&& error) :
        error_(std::forward<ErrorT>(error)),
        has_error_(true) {}

    [[nodiscard]]
    bool has_error() const noexcept {
        return has_error_;
    }

    [[nodiscard]]
    const ErrorT& error() const {
        if (!has_error()) {
            throw std::logic_error("Attempted to access error of an success Result.");
        }
        return error_;
    }

    ReturnT value() & {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<StoredT>) {
            return this->value_.get();
        }
        else {
            return this->value_;
        }
    }

    make_const_t<ReturnT> value() const & {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<StoredT>) {
            return this->value_.get();
        }
        else {
            return this->value_;
        }
    }

    ReturnT value() && {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<StoredT>) {
            return std::move(this->value_.get());
        }
        else {
            return std::move(this->value_);
        }
    }

    make_const_t<ReturnT> value() const && {
        if (this->has_error()) {
            detail::ThrowBadValueAccessException<ErrorT>();
        }
        if constexpr (std::is_reference_v<StoredT>) {
            return std::move(this->value_.get());
        }
        else {
            return std::move(this->value_);
        }
    }

    operator Result<StoredT, ErrorT>() const {
        if (has_error()) {
            return Result<StoredT, ErrorT>(error_);
        }
        if constexpr (std::is_reference_v<StoredT>) {
            return Result<StoredT, ErrorT>(std::forward<StoredT>(value_.get()));
        }
        else {
            return Result<StoredT, ErrorT>(std::forward<StoringT>(value_));
        }
    }
};

template <typename ErrorT>
struct ResultHolder<void, ErrorT> {
    using ErrorT = ErrorT;

private:
    std::optional<ErrorT> error_;

public:
    ResultHolder(ErrorT&& error) :
        error_(std::forward<ErrorT>(error)) {}

    [[nodiscard]]
    bool has_error() const noexcept {
        return error_.has_value();
    }

    [[nodiscard]]
    const ErrorT& error() const {
        if (!has_error()) {
            throw std::logic_error("Attempted to access error of an success Result.");
        }
        return error_.value();
    }

    operator Result<void, ErrorT>() const {
        if (has_error()) {
            return Result<void, ErrorT>(error_);
        }
        return Result<void, ErrorT>();
    }
};

}

#endif //RESCPP_RESULT_HOLDER_HPP
