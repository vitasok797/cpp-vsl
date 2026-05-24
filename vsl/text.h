#ifndef VSL_TEXT_H
#define VSL_TEXT_H

#include <vsl/regex.h>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <uni_algo/case.h>
#include <uni_algo/conv.h>

#include <algorithm>
#include <array>
#include <concepts>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

// Reference (uni-algo):
// https://github.com/uni-algo/uni-algo
// https://github.com/uni-algo/uni-algo/blob/main/doc/API.md

namespace vsl
{

using FoundStr = una::found;

inline auto is_valid_utf8(std::string_view str) -> bool
{
    return una::is_valid_utf8<char>(str);
}

inline auto is_ascii(std::string_view str) -> bool
{
    return std::ranges::all_of(str, [](char c) { return static_cast<unsigned char>(c) < 128; });
}

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

inline auto to_lowercase_ascii(std::string_view str) -> std::string
{
    auto res = std::string{};
    res.reserve(str.size());

    std::transform(str.begin(), str.end(), std::back_inserter(res),
                   [](unsigned char c) { return (c >= 'A' && c <= 'Z') ? (c + 32) : c; });

    return res;
}

inline auto to_uppercase_ascii(std::string_view str) -> std::string
{
    auto res = std::string{};
    res.reserve(str.size());

    std::transform(str.begin(), str.end(), std::back_inserter(res),
                   [](unsigned char c) { return (c >= 'a' && c <= 'z') ? (c - 32) : c; });

    return res;
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

inline constexpr auto trim_left_sv(std::string_view str) noexcept -> std::string_view
{
    const auto start = std::find_if_not(str.begin(), str.end(), detail::is_whitespace);
    return std::string_view(start, str.end());
}

inline constexpr auto trim_right_sv(std::string_view str) noexcept -> std::string_view
{
    const auto end = std::find_if_not(str.rbegin(), str.rend(), detail::is_whitespace).base();
    return std::string_view(str.begin(), end);
}

inline constexpr auto trim_sv(std::string_view str) noexcept -> std::string_view
{
    return trim_right_sv(trim_left_sv(str));
}

inline auto trim_left(std::string_view str) -> std::string
{
    return std::string{trim_left_sv(str)};
}

inline auto trim_right(std::string_view str) -> std::string
{
    return std::string{trim_right_sv(str)};
}

inline auto trim(std::string_view str) -> std::string
{
    return std::string{trim_sv(str)};
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

inline auto indent(std::string_view str, int width) -> std::string
{
    static const auto re = vsl::ReAscii{"^.+", Re::multiline};
    const auto repl = std::string(std::max(0, width), ' ') + "$&";
    return vsl::re_replace(str, re, repl);
}

inline auto to_lf(std::string_view str) -> std::string
{
    static const auto re = vsl::ReAscii{"\r\n"};
    return vsl::re_replace(str, re, "\n");
}

inline auto to_crlf(std::string_view str) -> std::string
{
    static const auto re = vsl::ReAscii{"(?<!\r)\n"};
    return vsl::re_replace(str, re, "\r\n");
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
