#ifndef RESCPP_RESULT_STORAGE_H
#define RESCPP_RESULT_STORAGE_H

#include <optional>
#include <type_traits>

#include "res-cpp/reference_wrapper.h"
#include "res-cpp/result_error.h"

namespace ResCpp {
template <typename T>
    requires (std::is_base_of_v<ResultErrorBase<T>, T>)
std::optional<T>& ResultErrorStorage() {
    using StoringT = std::optional<T>;
    static thread_local std::aligned_storage<sizeof(StoringT), alignof(StoringT)> storage;
    return *reinterpret_cast<StoringT*>(&storage);
}

template <typename T>
    requires (!std::is_base_of_v<ResultErrorBase<T>, T>
        && !std::is_reference_v<T>)
T& ResultStorage() {
    static thread_local std::aligned_storage<sizeof(T), alignof(T)> storage;
    return *reinterpret_cast<T*>(&storage);
}

template <typename T>
    requires (!std::is_base_of_v<ResultErrorBase<T>, T>
        && std::is_reference_v<T>)
T ResultStorage() {
    using StoringT = ReferenceWrapper<T>;
    static thread_local std::aligned_storage<sizeof(StoringT), alignof(StoringT)> storage;
    return reinterpret_cast<StoringT*>(&storage)->get();
}
}

#endif //RESCPP_RESULT_STORAGE_H
