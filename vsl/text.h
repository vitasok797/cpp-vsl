#ifndef VSL_TEXT_H
#define VSL_TEXT_H

#include <vsl/concepts.h>
#include <vsl/enum.h>
#include <vsl/types.h>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <uni_algo/case.h>

#include <algorithm>
#include <cassert>
#include <concepts>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace vsl
{

using FoundSubstr = una::found;

struct AsciiTag
{
    explicit AsciiTag() = default;
};

struct IgnoreCaseTag
{
    explicit IgnoreCaseTag() = default;
};

struct IgnoreAsciiCaseTag
{
    explicit IgnoreAsciiCaseTag() = default;
};

inline constexpr auto ascii = AsciiTag{};
inline constexpr auto ignore_case = IgnoreCaseTag{};
inline constexpr auto ignore_ascii_case = IgnoreAsciiCaseTag{};

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
inline auto to_upper(std::string_view str, AsciiTag) -> std::string
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
inline auto to_lower(std::string_view str, AsciiTag) -> std::string
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
inline auto is_equal(std::string_view str1, std::string_view str2, IgnoreCaseTag) -> bool
{
    return (una::caseless::compare_utf8<char>(str1, str2) == 0);
}

[[nodiscard]]
inline constexpr auto is_equal(std::string_view str1, std::string_view str2, IgnoreAsciiCaseTag) noexcept -> bool
{
    constexpr auto CHUNK_SIZE = ptrdiff_t{512};

    if (str1.size() != str2.size()) return false;

    const auto end1 = str1.end();
    auto it1 = str1.begin();
    auto it2 = str2.begin();

    while (it1 != end1)
    {
        const auto chunk_end1 = it1 + std::min(CHUNK_SIZE, std::distance(it1, end1));

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
inline auto compare_str(std::string_view str1, std::string_view str2, IgnoreCaseTag) -> int
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
inline auto collate_str(std::string_view str1, std::string_view str2, IgnoreCaseTag) -> int
{
    return una::caseless::collate_utf8<char>(str1, str2);
}

template<typename = void>
[[nodiscard]]
inline auto find_substr(std::string_view str1, std::string_view str2) -> FoundSubstr
{
    return una::casesens::find_utf8<char>(str1, str2);
}

template<typename = void>
[[nodiscard]]
inline auto find_substr(std::string_view str1, std::string_view str2, IgnoreCaseTag) -> FoundSubstr
{
    return una::caseless::find_utf8<char>(str1, str2);
}

[[nodiscard]]
inline constexpr auto starts_with(std::string_view str, std::string_view prefix, IgnoreAsciiCaseTag) noexcept -> bool
{
    if (str.size() < prefix.size()) return false;
    return is_equal(str.substr(0, prefix.size()), prefix, ignore_ascii_case);
}

[[nodiscard]]
inline constexpr auto ends_with(std::string_view str, std::string_view suffix, IgnoreAsciiCaseTag) noexcept -> bool
{
    if (str.size() < suffix.size()) return false;
    return is_equal(str.substr(str.size() - suffix.size()), suffix, ignore_ascii_case);
}

namespace detail
{

inline constexpr auto ASCII_WHITESPACES = std::string_view{" \n\r\t\f\v"};
inline constexpr auto NBSP = std::string_view{"\xC2\xA0"};

}  // namespace detail

[[nodiscard]]
inline constexpr auto trim_start(std::string_view str,
                                 AsciiTag,
                                 std::string_view symbols = detail::ASCII_WHITESPACES) noexcept -> std::string_view
{
    const auto first = str.find_first_not_of(symbols);
    if (first == std::string_view::npos) return {};
    str.remove_prefix(first);
    return str;
}

[[nodiscard]]
inline constexpr auto trim_end(std::string_view str,
                               AsciiTag,
                               std::string_view symbols = detail::ASCII_WHITESPACES) noexcept -> std::string_view
{
    const auto last = str.find_last_not_of(symbols);
    if (last == std::string_view::npos) return {};
    str.remove_suffix(str.size() - last - 1);
    return str;
}

[[nodiscard]]
inline constexpr auto trim(std::string_view str,
                           AsciiTag,
                           std::string_view symbols = detail::ASCII_WHITESPACES) noexcept -> std::string_view
{
    return trim_end(trim_start(str, ascii, symbols), ascii, symbols);
}

[[nodiscard]]
inline constexpr auto trim_start(std::string_view str) noexcept -> std::string_view
{
    while (true)
    {
        str = trim_start(str, ascii);
        if (!str.starts_with(detail::NBSP)) return str;
        str.remove_prefix(detail::NBSP.size());
    }
}

[[nodiscard]]
inline constexpr auto trim_end(std::string_view str) noexcept -> std::string_view
{
    while (true)
    {
        str = trim_end(str, ascii);
        if (!str.ends_with(detail::NBSP)) return str;
        str.remove_suffix(detail::NBSP.size());
    }
}

[[nodiscard]]
inline constexpr auto trim(std::string_view str) noexcept -> std::string_view
{
    return trim_end(trim_start(str));
}

[[nodiscard]]
inline constexpr auto trim_prefix(std::string_view str, std::string_view prefix) noexcept -> std::string_view
{
    if (str.starts_with(prefix)) str.remove_prefix(prefix.size());
    return str;
}

[[nodiscard]]
inline constexpr auto trim_suffix(std::string_view str, std::string_view suffix) noexcept -> std::string_view
{
    if (str.ends_with(suffix)) str.remove_suffix(suffix.size());
    return str;
}

[[nodiscard]]
inline constexpr auto trim_prefix(std::string_view str, std::string_view prefix, IgnoreAsciiCaseTag) noexcept
    -> std::string_view
{
    if (starts_with(str, prefix, ignore_ascii_case)) str.remove_prefix(prefix.size());
    return str;
}

[[nodiscard]]
inline constexpr auto trim_suffix(std::string_view str, std::string_view suffix, IgnoreAsciiCaseTag) noexcept
    -> std::string_view
{
    if (ends_with(str, suffix, ignore_ascii_case)) str.remove_suffix(suffix.size());
    return str;
}

template<typename OutputStrType = std::string>
    requires vsl::one_of_type<OutputStrType, std::string, std::string_view>
inline auto split(std::vector<OutputStrType>& out,
                  std::string_view str,
                  std::string_view separator,
                  SplitOptions opt = SplitOptions::NONE,
                  vsl::Index max_tokens = std::numeric_limits<vsl::Index>::max()) -> void
{
    if (max_tokens <= 0) return;

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
        // NOTE: When 'max_tokens' is reached, the last token takes the entire remaining string.
        // TRIM and SKIP_EMPTY options are applied to it as usual.
        const auto sep_pos = (max_tokens > 1) ? str.find(separator, pos) : std::string_view::npos;

        auto token = (sep_pos == std::string_view::npos) ? str.substr(pos) : str.substr(pos, sep_pos - pos);

        if (trim_tokens)
        {
            token = vsl::trim(token);
        }

        const auto keep_token = !(token.empty() && skip_empty);
        if (keep_token)
        {
            out.emplace_back(token);
            --max_tokens;
        }

        if (sep_pos == std::string_view::npos) break;
        pos = sep_pos + separator.size();
    }
}

template<typename OutputStrType = std::string>
    requires vsl::one_of_type<OutputStrType, std::string, std::string_view>
[[nodiscard]]
inline auto split(std::string_view str,
                  std::string_view separator,
                  SplitOptions opt = SplitOptions::NONE,
                  vsl::Index max_tokens = std::numeric_limits<vsl::Index>::max()) -> std::vector<OutputStrType>
{
    auto res = std::vector<OutputStrType>{};
    split(res, str, separator, opt, max_tokens);
    return res;
}

namespace detail
{

template<typename T>
concept replacement_str_type = vsl::string_like<T>;

template<typename T>
concept replacement_func_type =
    std::invocable<T, std::string_view> && vsl::string_like<std::invoke_result_t<T, std::string_view>>;

template<typename T>
concept replacement_any_type = replacement_str_type<T> || replacement_func_type<T>;

struct FindSubstrPolicy
{
    constexpr auto operator()(std::string_view str, std::string_view substring) const noexcept
        -> std::optional<std::string_view>
    {
        if (str.empty() || substring.empty()) return std::nullopt;

        const auto pos = str.find(substring);
        if (pos == std::string_view::npos) return std::nullopt;
        return str.substr(pos, substring.size());
    }
};

struct FindSubstrAsciiCaselessPolicy
{
    constexpr auto operator()(std::string_view str, std::string_view substring) const noexcept
        -> std::optional<std::string_view>
    {
        if (str.empty() || substring.empty()) return std::nullopt;

        constexpr auto char_to_lower = [](char c) noexcept -> unsigned char
        {
            const auto uc = static_cast<unsigned char>(c);
            return (uc >= 'A' && uc <= 'Z') ? static_cast<unsigned char>(uc + ('a' - 'A')) : uc;
        };

        const auto found_rng = std::ranges::search(str, substring, {}, char_to_lower, char_to_lower);
        if (found_rng.empty()) return std::nullopt;
        return std::string_view(found_rng.begin(), found_rng.end());
    }
};

template<typename FindPolicy, replacement_any_type Replacement>
inline auto replace_impl(
    std::string& out, std::string_view str, std::string_view pattern, const Replacement& replacement, vsl::Index n)
    -> void
{
    if (str.empty() || pattern.empty() || n <= 0)
    {
        out.append(str);
        return;
    }

    const auto find_policy = FindPolicy{};

    while (n > 0)
    {
        const auto match = find_policy(str, pattern);
        if (!match.has_value()) break;
        assert(!match->empty());

        const auto head_length = match->data() - str.data();
        out.append(str.data(), vsl::as_unsigned(head_length));

        if constexpr (replacement_func_type<Replacement>)
        {
            out.append(std::invoke(replacement, *match));
        }
        else
        {
            out.append(replacement);
        }

        const auto tail_start = match->data() + match->size();
        const auto tail_end = str.data() + str.size();
        const auto tail_length = tail_end - tail_start;
        str = std::string_view(tail_start, vsl::as_unsigned(tail_length));

        --n;
    }

    out.append(str);
}

template<typename FindPolicy>
inline constexpr auto contains_substr_impl(std::string_view str, std::string_view substring) noexcept -> bool
{
    // Return true for an empty substring to match C++23 std::string_view::contains behavior
    if (substring.empty()) return true;

    return FindPolicy{}(str, substring).has_value();
}

}  // namespace detail

template<detail::replacement_any_type Replacement>
inline auto replace(std::string& out,
                    std::string_view str,
                    std::string_view pattern,
                    const Replacement& replacement,
                    vsl::Index n = std::numeric_limits<vsl::Index>::max()) -> void
{
    detail::replace_impl<detail::FindSubstrPolicy>(out, str, pattern, replacement, n);
}

template<detail::replacement_any_type Replacement>
[[nodiscard]]
inline auto replace(std::string_view str,
                    std::string_view pattern,
                    const Replacement& replacement,
                    vsl::Index n = std::numeric_limits<vsl::Index>::max()) -> std::string
{
    auto res = std::string{};
    res.reserve(str.size() + str.size() / 5);  // +20%
    replace(res, str, pattern, replacement, n);
    return res;
}

template<detail::replacement_any_type Replacement>
inline auto replace(std::string& out,
                    std::string_view str,
                    std::string_view pattern,
                    const Replacement& replacement,
                    IgnoreAsciiCaseTag,
                    vsl::Index n = std::numeric_limits<vsl::Index>::max()) -> void
{
    detail::replace_impl<detail::FindSubstrAsciiCaselessPolicy>(out, str, pattern, replacement, n);
}

template<detail::replacement_any_type Replacement>
[[nodiscard]]
inline auto replace(std::string_view str,
                    std::string_view pattern,
                    const Replacement& replacement,
                    IgnoreAsciiCaseTag,
                    vsl::Index n = std::numeric_limits<vsl::Index>::max()) -> std::string
{
    auto res = std::string{};
    res.reserve(str.size() + str.size() / 5);  // +20%
    replace(res, str, pattern, replacement, ignore_ascii_case, n);
    return res;
}

// NOTE: If targeting C++23, prefer std::string_view::contains for case-sensitive checks
[[nodiscard]]
inline constexpr auto contains_substr(std::string_view str, std::string_view substring) noexcept -> bool
{
#ifdef __cpp_lib_string_contains
    return str.contains(substring);
#else
    return detail::contains_substr_impl<detail::FindSubstrPolicy>(str, substring);
#endif
}

[[nodiscard]]
inline constexpr auto contains_substr(std::string_view str, std::string_view substring, IgnoreAsciiCaseTag) noexcept
    -> bool
{
    return detail::contains_substr_impl<detail::FindSubstrAsciiCaselessPolicy>(str, substring);
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
            out.append(vsl::as_unsigned_unchecked(width), ' ');
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
    for (auto i = size_t{0}; i < str.size(); ++i)
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
    for (auto i = size_t{0}; i < str.size(); ++i)
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

[[nodiscard]]
inline auto repeat_str(std::string_view str, vsl::Index n) -> std::string
{
    if (n <= 0 || str.empty())
    {
        return {};
    }

    auto res = std::string{};
    if (str.size() > res.max_size() / vsl::as_unsigned_unchecked(n)) [[unlikely]]
    {
        throw std::length_error{"repeat_str: resulting string size exceeds max_size()"};
    }
    res.reserve(str.size() * vsl::as_unsigned_unchecked(n));

    for (auto i = vsl::Index{0}; i < n; ++i)
    {
        res.append(str);
    }

    return res;
}

}  // namespace vsl

#endif  // VSL_TEXT_H
