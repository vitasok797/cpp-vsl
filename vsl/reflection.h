#ifndef VSL_REFLECTION_H
#define VSL_REFLECTION_H

#include <./nameof.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <cstdint>
#include <string_view>

// Reference:
// https://github.com/Neargye/nameof/blob/master/doc/reference.md

template<std::uint16_t N, typename Char>
struct fmt::range_format_kind<nameof::cstring<N>, Char>
    : std::integral_constant<fmt::range_format, fmt::range_format::disabled>
{};

namespace nameof
{

template<std::uint16_t N>
auto format_as(const nameof::cstring<N>& obj) -> std::string_view
{
    return obj;
}

}  // namespace nameof

#endif  // VSL_REFLECTION_H
