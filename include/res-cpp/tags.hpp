#ifndef RESCPP_TAGS_H
#define RESCPP_TAGS_H

namespace ResCpp::detail {
struct OkTag {};

struct ErrorTag {};

template <typename ErrorT>
struct PassErrorTag {};

inline constexpr OkTag Ok;
inline constexpr ErrorTag Error;
}

#endif //RESCPP_TAGS_H
