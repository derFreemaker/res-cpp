#ifndef RESCPP_TAGS_H
#define RESCPP_TAGS_H

namespace ResCpp::detail {
struct ErrorTag {};

struct PassErrorTag {};

inline constexpr ErrorTag Error{};
inline constexpr PassErrorTag PassError{};
}

#endif //RESCPP_TAGS_H
