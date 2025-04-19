#ifndef RESCPP_REFERENCE_WRAPPER_H
#define RESCPP_REFERENCE_WRAPPER_H

#include <memory>
#include <type_traits>

namespace ResCpp {
template <typename T>
struct ReferenceWrapper;

template <typename T>
struct ReferenceWrapper<T&> {
private:
    T* data_;

public:
    explicit constexpr ReferenceWrapper(T& data) noexcept
        : data_(&data) {}

    constexpr T& get() const noexcept {
        return static_cast<T&>(*data_);
    }

    constexpr operator T&() const noexcept {
        return get();
    }
};

template <typename T>
struct ReferenceWrapper<const T&> {
private:
    T* data_;

public:
    explicit constexpr ReferenceWrapper(const T& data) noexcept
        : data_(&const_cast<T&>(data)) {}

    constexpr const T& get() const noexcept {
        return static_cast<T&>(*data_);
    }

    constexpr operator const T&() const noexcept {
        return get();
    }
};
}

#endif //RESCPP_REFERENCE_WRAPPER_H
