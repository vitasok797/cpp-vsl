#ifndef VSL_TEXT_H
#define VSL_TEXT_H

#include <fmt/format.h>
#include <Poco/RegularExpression.h>

#include <algorithm>
#include <concepts>
#include <string>
#include <string_view>
#include <utility>

namespace vsl
{

inline auto indent(std::string_view text, int width) -> std::string
{
    using Re = Poco::RegularExpression;

    // TODO: remove this POCO bug workaround
    // RE_NEWLINE_ANYCRLF cannot be used until POCO version 1.15.0
    // see https://github.com/pocoproject/poco/issues/4870
    static const auto re = Re{"^.+", Re::RE_MULTILINE | Re::RE_NEWLINE_ANY};

    auto result = std::string{text};
    auto repl = fmt::format("{: <{}}$0", "", std::max(0, width));
    re.subst(result, repl, Re::RE_GLOBAL);
    return result;
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
