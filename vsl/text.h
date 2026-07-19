#ifndef VSL_TEXT_H
#define VSL_TEXT_H

#include <vsl/concepts.h>
#include <vsl/enum.h>
#include <vsl/types.h>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <uni_algo/case.h>

#include <algorithm>
#include <concepts>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace vsl
{

using FoundStr = una::found;

enum class SplitOptions : u32
{
    NONE = 0,
    TRIM = 1 << 0,
    SKIP_EMPTY = 1 << 1,
};
VSL_DECLARE_ENUM_FLAGS(SplitOptions)

inline constexpr auto LF = std::string_view{"\n"};
inline constexpr auto CRLF = std::string_view{"\r\n"};

template<typename = void>
[[nodiscard]]
inline auto to_upper(std::string_view str) -> std::string
{
    return una::cases::to_uppercase_utf8<char>(str);
}

[[nodiscard]]
inline auto to_upper_ascii(std::string_view str) -> std::string
{
    auto res = std::string{str};
    for (char& c : res)
    {
        if (c >= 'a' && c <= 'z')
        {
            c -= ('a' - 'A');
        }
    }
    return res;
}

template<typename = void>
[[nodiscard]]
inline auto to_lower(std::string_view str) -> std::string
{
    return una::cases::to_lowercase_utf8<char>(str);
}

[[nodiscard]]
inline auto to_lower_ascii(std::string_view str) -> std::string
{
    auto res = std::string{str};
    for (char& c : res)
    {
        if (c >= 'A' && c <= 'Z')
        {
            c += ('a' - 'A');
        }
    }
    return res;
}

template<typename = void>
[[nodiscard]]
inline auto to_casefold(std::string_view str) -> std::string
{
    return una::cases::to_casefold_utf8<char>(str);
}

template<typename = void>
[[nodiscard]]
inline auto to_titlecase(std::string_view str) -> std::string
{
    return una::cases::to_titlecase_utf8<char>(str);
}

template<typename = void>
[[nodiscard]]
inline auto is_equal(std::string_view str1, std::string_view str2) -> bool
{
    return (una::casesens::compare_utf8<char>(str1, str2) == 0);
}

template<typename = void>
[[nodiscard]]
inline auto is_equal_icase(std::string_view str1, std::string_view str2) -> bool
{
    return (una::caseless::compare_utf8<char>(str1, str2) == 0);
}

inline constexpr auto IS_EQUAL_ASCII_ICASE_CHUNK_SIZE = 512;

[[nodiscard]]
inline constexpr auto is_equal_ascii_icase(std::string_view str1, std::string_view str2) noexcept -> bool
{
    if (str1.size() != str2.size()) return false;

    const auto end1 = str1.end();
    auto it1 = str1.begin();
    auto it2 = str2.begin();

    while (it1 != end1)
    {
        const auto chunk_end1 =
            it1 + std::min(static_cast<ptrdiff_t>(IS_EQUAL_ASCII_ICASE_CHUNK_SIZE), std::distance(it1, end1));

        std::tie(it1, it2) = std::mismatch(it1, chunk_end1, it2);

        while (it1 != chunk_end1)
        {
            const auto c1 = static_cast<unsigned char>(*it1);
            const auto c2 = static_cast<unsigned char>(*it2);
            if (c1 != c2)
            {
                const auto lc1 = c1 | 32;
                const auto lc2 = c2 | 32;
                if (lc1 != lc2 || lc1 < 'a' || lc1 > 'z')
                {
                    return false;
                }
            }

            ++it1;
            ++it2;
        }
    }

    return true;
}

template<typename = void>
[[nodiscard]]
inline auto compare_str(std::string_view str1, std::string_view str2) -> int
{
    return una::casesens::compare_utf8<char>(str1, str2);
}

template<typename = void>
[[nodiscard]]
inline auto compare_str_icase(std::string_view str1, std::string_view str2) -> int
{
    return una::caseless::compare_utf8<char>(str1, str2);
}

template<typename = void>
[[nodiscard]]
inline auto collate_str(std::string_view str1, std::string_view str2) -> int
{
    return una::casesens::collate_utf8<char>(str1, str2);
}

template<typename = void>
[[nodiscard]]
inline auto collate_str_icase(std::string_view str1, std::string_view str2) -> int
{
    return una::caseless::collate_utf8<char>(str1, str2);
}

template<typename = void>
[[nodiscard]]
inline auto find_str(std::string_view str1, std::string_view str2) -> FoundStr
{
    return una::casesens::find_utf8<char>(str1, str2);
}

template<typename = void>
[[nodiscard]]
inline auto find_str_icase(std::string_view str1, std::string_view str2) -> FoundStr
{
    return una::caseless::find_utf8<char>(str1, str2);
}

namespace detail
{

inline constexpr auto ASCII_WHITESPACES = std::string_view{" \n\r\t\f\v"};
inline constexpr auto NBSP = std::string_view{"\xC2\xA0"};

}  // namespace detail

[[nodiscard]]
inline constexpr auto trim_ascii_start(std::string_view str,
                                       std::string_view symbols = detail::ASCII_WHITESPACES) noexcept
    -> std::string_view
{
    const auto first = str.find_first_not_of(symbols);
    if (first == std::string_view::npos) return {};
    str.remove_prefix(first);
    return str;
}

[[nodiscard]]
inline constexpr auto trim_ascii_end(std::string_view str,
                                     std::string_view symbols = detail::ASCII_WHITESPACES) noexcept -> std::string_view
{
    const auto last = str.find_last_not_of(symbols);
    if (last == std::string_view::npos) return {};
    str.remove_suffix(str.size() - last - 1);
    return str;
}

[[nodiscard]]
inline constexpr auto trim_ascii(std::string_view str, std::string_view symbols = detail::ASCII_WHITESPACES) noexcept
    -> std::string_view
{
    return trim_ascii_end(trim_ascii_start(str, symbols), symbols);
}

[[nodiscard]]
inline constexpr auto trim_start(std::string_view str) noexcept -> std::string_view
{
    while (true)
    {
        str = trim_ascii_start(str);
        if (!str.starts_with(detail::NBSP)) return str;
        str.remove_prefix(detail::NBSP.size());
    }
}

[[nodiscard]]
inline constexpr auto trim_end(std::string_view str) noexcept -> std::string_view
{
    while (true)
    {
        str = trim_ascii_end(str);
        if (!str.ends_with(detail::NBSP)) return str;
        str.remove_suffix(detail::NBSP.size());
    }
}

[[nodiscard]]
inline constexpr auto trim(std::string_view str) noexcept -> std::string_view
{
    return trim_end(trim_start(str));
}

template<typename OutputStrType = std::string>
    requires vsl::one_of_type<OutputStrType, std::string, std::string_view>
inline auto split(std::vector<OutputStrType>& out,
                  std::string_view str,
                  std::string_view separator,
                  SplitOptions opt = SplitOptions::NONE) -> void
{
    const auto trim_tokens = vsl::enum_contains_flags(opt, SplitOptions::TRIM);
    const auto skip_empty = vsl::enum_contains_flags(opt, SplitOptions::SKIP_EMPTY);

    if (separator.empty())
    {
        if (trim_tokens)
        {
            str = vsl::trim(str);
        }
        if (!str.empty() || !skip_empty)
        {
            out.emplace_back(str);
        }
        return;
    }

    auto pos = size_t{0};
    while (true)
    {
        const auto sep_pos = str.find(separator, pos);
        auto token = (sep_pos == std::string_view::npos) ? str.substr(pos) : str.substr(pos, sep_pos - pos);

        if (trim_tokens)
        {
            token = vsl::trim(token);
        }

        const auto keep_token = !(token.empty() && skip_empty);
        if (keep_token)
        {
            out.emplace_back(token);
        }

        if (sep_pos == std::string_view::npos) break;
        pos = sep_pos + separator.size();
    }
}

template<typename OutputStrType = std::string>
    requires vsl::one_of_type<OutputStrType, std::string, std::string_view>
[[nodiscard]]
inline auto split(std::string_view str, std::string_view separator, SplitOptions opt = SplitOptions::NONE)
    -> std::vector<OutputStrType>
{
    auto res = std::vector<OutputStrType>{};
    split(res, str, separator, opt);
    return res;
}

template<std::ranges::input_range R>
[[nodiscard]]
inline auto join(const R& items, std::string_view separator = ", ", std::string_view fmt_spec = "{}") -> std::string
{
    return fmt::format(fmt::runtime(fmt_spec), fmt::join(items, separator));
}

inline auto indent(std::string& out, std::string_view str, int width) -> void
{
    if (width <= 0)
    {
        out.append(str);
        return;
    }

    auto pos = size_t{0};
    while (true)
    {
        const auto eol_start = str.find_first_of("\r\n", pos);
        const auto text = (eol_start == std::string_view::npos) ? str.substr(pos) : str.substr(pos, eol_start - pos);
        if (!text.empty())
        {
            out.append(static_cast<size_t>(width), ' ');
            out.append(text);
        }
        pos = eol_start;
        if (pos == std::string_view::npos) break;

        const auto next_start = str.find_first_not_of("\r\n", pos);
        const auto eol_text =
            (next_start == std::string_view::npos) ? str.substr(pos) : str.substr(pos, next_start - pos);
        out.append(eol_text);
        pos = next_start;
        if (pos == std::string_view::npos) break;
    }
}

[[nodiscard]]
inline auto indent(std::string_view str, int width) -> std::string
{
    auto res = std::string{};
    res.reserve(str.size() + str.size() / 2);  // +50%
    indent(res, str, width);
    return res;
}

[[nodiscard]]
inline auto to_lf(std::string_view str) -> std::string
{
    auto res = std::string{};
    res.reserve(str.size());

    auto start = size_t{0};
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '\r')
        {
            res.append(str, start, i - start);
            res.push_back('\n');
            if (i + 1 < str.size() && str[i + 1] == '\n')
            {
                ++i;
            }
            start = i + 1;
        }
    }
    res.append(str, start);

    return res;
}

inline auto to_crlf(std::string& out, std::string_view str) -> void
{
    auto start = size_t{0};
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '\r' || str[i] == '\n')
        {
            out.append(str, start, i - start);
            out.append("\r\n");
            if (str[i] == '\r' && i + 1 < str.size() && str[i + 1] == '\n')
            {
                ++i;
            }
            start = i + 1;
        }
    }
    out.append(str, start);
}

[[nodiscard]]
inline auto to_crlf(std::string_view str) -> std::string
{
    auto res = std::string{};
    res.reserve(str.size() + str.size() / 5);  // +20%
    to_crlf(res, str);
    return res;
}

template<std::integral CountType, std::integral TotalCountType>
[[nodiscard]]
inline auto format_count_of_total(CountType count, TotalCountType total_count, std::string_view separator = "/")
    -> std::string
{
    auto res = std::string{};
    if (!std::cmp_equal(count, total_count))
    {
        res += std::to_string(count);
        res.append(separator);
    }
    res += std::to_string(total_count);
    return res;
}

}  // namespace vsl

#endif  // VSL_TEXT_H
