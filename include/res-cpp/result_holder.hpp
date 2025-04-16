#ifndef RESCPP_RESULT_HOLDER_HPP
#define RESCPP_RESULT_HOLDER_HPP

#include "res-cpp/type_traits.hpp"
#include "res-cpp/result.hpp"

namespace ResCpp {

template <typename ResultT>
struct ResultHolder {
    using StoredT = typename ResultT::StoredT;
    using ErrorT = typename ResultT::ErrorT;
    using StoringT = typename ResultT::StoringT;
    using ReturnT = typename ResultT::ReturnT;

private:
    union {
        ErrorT error_;
        StoringT value_;
    };

    bool has_error_;

public:
    ResultHolder(const ResultT& result) noexcept {
        has_error_ = result.has_error();
        if (has_error_) {
            std::memcpy(&error_, &ResultErrorStorage<ErrorT>(), sizeof(ErrorT));
        }
        else {
            std::memcpy(&value_, &ResultStorage<StoringT>(), sizeof(StoringT));
        }
    }

    ~ResultHolder() noexcept {
        if (!has_error_) {
            value_.StoringT::~StoringT();
        }
    }
    
    [[nodiscard]]
    bool has_error() const noexcept {
        return has_error_;
    }

    [[nodiscard]]
    const ErrorT& error() const {
        if (!has_error()) {
            detail::ThrowBadErrorAccessException();
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
            return Result<StoredT, ErrorT>(value_.get());
        }
        else {
            return Result<StoredT, ErrorT>(value_);
        }
    }
};

template <typename ErrorT>
struct ResultHolder<Result<void, ErrorT>> {
    using ErrorT = ErrorT;

private:
    std::optional<ErrorT> error_;

public:
    ResultHolder(const Result<void, ErrorT>& result) {
        if (result.has_error()) {
            error_ = result.error();
        }
    }
    
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
