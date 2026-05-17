#ifndef VSL_TEXT_H
#define VSL_TEXT_H

#include <vsl/regex.h>

#include <fmt/format.h>

#include <algorithm>
#include <concepts>
#include <string>
#include <string_view>
#include <utility>

namespace vsl
{

inline auto indent(std::string_view text, int width) -> std::string
{
    static const auto re = vsl::ReAscii{"^.+", Re::multiline};
    const auto repl = std::string(std::max(0, width), ' ') + "$&";
    return vsl::re_replace(text, re, repl);
}

template<std::integral CountType, std::integral TotalCountType>
auto out_of(CountType count, TotalCountType total_count) -> std::string
{
    if (std::cmp_equal(count, total_count))
    {
        return fmt::format("{}", count);
    }
    else
    {
        return fmt::format("{}/{}", count, total_count);
    }
}

}  // namespace vsl

#endif  // VSL_TEXT_H
