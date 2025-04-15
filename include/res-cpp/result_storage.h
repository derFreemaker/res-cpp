#ifndef RESCPP_RESULT_STORAGE_H
#define RESCPP_RESULT_STORAGE_H

#include <optional>
#include <type_traits>

#include "res-cpp/result_error.h"


namespace ResCpp {
template <typename T>
    requires (std::is_base_of_v<ResultErrorBase<T>, T>)
std::optional<T>& ResultErrorStorage() {
    using StoringT = std::optional<T>;
    struct alignas(StoringT) storage_type {
        std::byte _[sizeof(StoringT)];
    };
    static thread_local storage_type storage;
    return *reinterpret_cast<StoringT*>(&storage);
}

template <typename T>
    requires (!std::is_base_of_v<ResultErrorBase<T>, T>
        && !std::is_reference_v<T>)
T& ResultStorage() {
    struct alignas(T) storage_type {
        std::byte _[sizeof(T)];
    };
    static thread_local storage_type storage;
    return *reinterpret_cast<T*>(&storage);
}
}

#endif //RESCPP_RESULT_STORAGE_H
