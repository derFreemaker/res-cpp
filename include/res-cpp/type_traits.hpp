#ifndef RESCPP_TYPE_TRAITS_H
#define RESCPP_TYPE_TRAITS_H

#include <type_traits>

namespace ResCpp {
namespace detail {
template <typename T>
struct make_const {
    using type = std::add_const_t<std::remove_const_t<T>>;
};

template <typename T>
struct make_const<T&> {
    using type = typename make_const<T>::type&;
};

template <typename T>
struct make_const<T&&> {
    using type = typename make_const<T>::type&&;
};
}

template <typename T>
using make_const_t = typename detail::make_const<T>::type;

template <typename T>
using make_lvalue_reference_t = std::remove_reference_t<T>&;

}

#endif //RESCPP_TYPE_TRAITS_H
