#ifndef RESCPP_TAGS_H
#define RESCPP_TAGS_H

namespace ResCpp::detail {
struct OkTag {};

struct ErrorTag {};

inline constexpr OkTag Ok;
inline constexpr ErrorTag Error;
}

#endif //RESCPP_TAGS_H
