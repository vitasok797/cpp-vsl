#ifndef VSL_TEXT_H
#define VSL_TEXT_H

#include <vsl/enum.h>
#include <vsl/regex.h>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <uni_algo/case.h>

#include <algorithm>
#include <array>
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

inline constexpr auto make_whitespace_lut() -> auto
{
    auto lut = std::array<bool, 256>{};
    lut[static_cast<unsigned char>(' ')] = true;
    lut[static_cast<unsigned char>('\t')] = true;
    lut[static_cast<unsigned char>('\n')] = true;
    lut[static_cast<unsigned char>('\r')] = true;
    lut[static_cast<unsigned char>('\v')] = true;
    lut[static_cast<unsigned char>('\f')] = true;
    return lut;
}

inline constexpr auto whitespace_lut = make_whitespace_lut();

inline constexpr auto is_whitespace(char c) noexcept -> bool
{
    return whitespace_lut[static_cast<unsigned char>(c)];
}

}  // namespace detail

inline constexpr auto trim_left(std::string_view str) noexcept -> std::string_view
{
    const auto start = std::find_if_not(str.begin(), str.end(), detail::is_whitespace);
    return std::string_view(start, str.end());
}

inline constexpr auto trim_right(std::string_view str) noexcept -> std::string_view
{
    const auto end = std::find_if_not(str.rbegin(), str.rend(), detail::is_whitespace).base();
    return std::string_view(str.begin(), end);
}

inline constexpr auto trim(std::string_view str) noexcept -> std::string_view
{
    return trim_right(trim_left(str));
}

inline auto split(std::string_view str, std::string_view separator, SplitOptions opt = SplitOptions::NONE)
    -> std::vector<std::string_view>
{
    const auto pattern = vsl::re_escape(separator);
    const auto re = vsl::Re{pattern};
    auto tokens = vsl::re_split(str, re, static_cast<vsl::ReSplitOptions>(opt));
    return std::vector(tokens.begin(), tokens.end());
}

template<std::ranges::input_range R>
auto join(const R& items, std::string_view separator = ", ") -> std::string
{
    return fmt::format("{}", fmt::join(items, separator));
}

template<std::ranges::input_range R>
auto join_fmt(const R& items, std::string_view fmt, std::string_view separator = ", ") -> std::string
{
    const auto full_fmt = fmt::format("{{{}}}", fmt);
    return fmt::format(fmt::runtime(full_fmt), fmt::join(items, separator));
}

namespace detail
{

inline const auto re_match_line = vsl::ReAscii{"^.+", Re::multiline};
inline const auto re_match_crlf = vsl::ReAscii{"\r\n"};
inline const auto re_match_lf = vsl::ReAscii{"(?<!\r)\n"};

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
    vsl::re_replace(out, str, detail::re_match_crlf, LF);
}

inline auto to_lf(std::string_view str) -> std::string
{
    return vsl::re_replace(str, detail::re_match_crlf, LF);
}

inline auto to_crlf(std::string& out, std::string_view str) -> void
{
    vsl::re_replace(out, str, detail::re_match_lf, CRLF);
}

inline auto to_crlf(std::string_view str) -> std::string
{
    return vsl::re_replace(str, detail::re_match_lf, CRLF);
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
