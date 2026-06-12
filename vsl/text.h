#ifndef VSL_TEXT_H
#define VSL_TEXT_H

#include <vsl/concepts.h>
#include <vsl/enum.h>
#include <vsl/regex.h>
#include <vsl/text_trim.h>
#include <vsl/types.h>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <uni_algo/case.h>

#include <algorithm>
#include <concepts>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace vsl
{

using FoundStr = una::found;

enum class SplitOptions : u32
{
    NONE = static_cast<u32>(ReSplitOptions::NONE),
    TRIM = static_cast<u32>(ReSplitOptions::TRIM),
    SKIP_EMPTY = static_cast<u32>(ReSplitOptions::SKIP_EMPTY),
};
VSL_DECLARE_ENUM_FLAGS(SplitOptions)

inline constexpr auto LF = "\n";
inline constexpr auto CRLF = "\r\n";

inline auto to_casefold(std::string_view str) -> std::string
{
    return una::cases::to_casefold_utf8<char>(str);
}

inline auto to_lowercase(std::string_view str) -> std::string
{
    return una::cases::to_lowercase_utf8<char>(str);
}

inline auto to_uppercase(std::string_view str) -> std::string
{
    return una::cases::to_uppercase_utf8<char>(str);
}

inline auto to_titlecase(std::string_view str) -> std::string
{
    return una::cases::to_titlecase_utf8<char>(str);
}

inline auto compare_str(std::string_view str1, std::string_view str2) -> bool
{
    return (una::casesens::compare_utf8<char>(str1, str2) == 0);
}

inline auto compare_str_icase(std::string_view str1, std::string_view str2) -> bool
{
    return (una::caseless::compare_utf8<char>(str1, str2) == 0);
}

inline auto collate(std::string_view str1, std::string_view str2) -> int
{
    return una::casesens::collate_utf8<char>(str1, str2);
}

inline auto collate_icase(std::string_view str1, std::string_view str2) -> int
{
    return una::caseless::collate_utf8<char>(str1, str2);
}

inline auto find_str(std::string_view str1, std::string_view str2) -> FoundStr
{
    return una::casesens::find_utf8<char>(str1, str2);
}

inline auto find_str_icase(std::string_view str1, std::string_view str2) -> FoundStr
{
    return una::caseless::find_utf8<char>(str1, str2);
}

namespace detail
{

template<typename OutputStr>
    requires vsl::one_of_type<OutputStr, std::string, std::string_view>
auto split_impl(std::string_view str, std::string_view separator, SplitOptions opt, size_t capacity_reserve)
    -> std::vector<OutputStr>
{
    const auto pattern = vsl::re_escape(separator);
    const auto re = vsl::Re{pattern};
    auto tokens = vsl::re_split(str, re, static_cast<vsl::ReSplitOptions>(opt));

    auto res = std::vector<OutputStr>{};
    res.reserve(capacity_reserve);

    if constexpr (vsl::same_type_as<OutputStr, std::string>)
    {
        auto string_tokens = tokens | std::views::transform([](std::string_view sv) { return std::string(sv); });
        res.assign(string_tokens.begin(), string_tokens.end());
    }
    else
    {
        res.assign(tokens.begin(), tokens.end());
    }

    return res;
}

}  // namespace detail

inline auto split(std::string_view str,
                  std::string_view separator,
                  SplitOptions opt = SplitOptions::NONE,
                  size_t capacity_reserve = 0) -> std::vector<std::string>
{
    return detail::split_impl<std::string>(str, separator, opt, capacity_reserve);
}

inline auto split_sv(std::string_view str,
                     std::string_view separator,
                     SplitOptions opt = SplitOptions::NONE,
                     size_t capacity_reserve = 0) -> std::vector<std::string_view>
{
    return detail::split_impl<std::string_view>(str, separator, opt, capacity_reserve);
}

template<std::ranges::input_range R>
auto join(const R& items, std::string_view separator = ", ") -> std::string
{
    return fmt::format("{}", fmt::join(items, separator));
}

template<std::ranges::input_range R>
auto join_f(const R& items, std::string_view fmt_spec, std::string_view separator = ", ") -> std::string
{
    if (fmt_spec.starts_with('{')) fmt_spec.remove_prefix(1);
    if (fmt_spec.ends_with('}')) fmt_spec.remove_suffix(1);
    const auto format_string = fmt::format("{{{}}}", fmt_spec);
    return fmt::format(fmt::runtime(format_string), fmt::join(items, separator));
}

namespace detail
{

inline const auto re_match_line = vsl::ReAscii{"^.+", Re::multiline};
inline const auto re_match_newline = vsl::ReAscii{"\\r?\\n"};

inline auto get_indent_line_repl(int width) -> std::string
{
    return std::string(std::max(0, width), ' ') + "$&";
}

}  // namespace detail

inline auto indent(std::string& out, std::string_view str, int width) -> void
{
    vsl::re_replace(out, str, detail::re_match_line, detail::get_indent_line_repl(width));
}

inline auto indent(std::string_view str, int width) -> std::string
{
    return vsl::re_replace(str, detail::re_match_line, detail::get_indent_line_repl(width));
}

inline auto to_lf(std::string& out, std::string_view str) -> void
{
    vsl::re_replace(out, str, detail::re_match_newline, LF);
}

inline auto to_lf(std::string_view str) -> std::string
{
    return vsl::re_replace(str, detail::re_match_newline, LF);
}

inline auto to_crlf(std::string& out, std::string_view str) -> void
{
    vsl::re_replace(out, str, detail::re_match_newline, CRLF);
}

inline auto to_crlf(std::string_view str) -> std::string
{
    return vsl::re_replace(str, detail::re_match_newline, CRLF);
}

template<std::integral CountType, std::integral TotalCountType>
auto out_of(CountType count, TotalCountType total_count, std::string_view separator = "/") -> std::string
{
    if (std::cmp_equal(count, total_count))
    {
        return fmt::format("{}", count);
    }
    else
    {
        return fmt::format("{}{}{}", count, separator, total_count);
    }
}

}  // namespace vsl

#endif  // VSL_TEXT_H
