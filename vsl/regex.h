#ifndef VSL_REGEX_H
#define VSL_REGEX_H

#include <vsl/concepts.h>
#include <vsl/enum.h>
#include <vsl/text.h>
#include <vsl/types.h>

#include <srell/srell.hpp>

#include <functional>
#include <initializer_list>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

// Reference:
// https://en.cppreference.com/cpp/regex
// https://www.akenotsuki.com/misc/srell/en/

namespace vsl
{

using ReError = srell::regex_error;

using Re = srell::u8cregex;
using ReAscii = srell::regex;

using ReMatch = srell::match_results<std::string_view::const_iterator>;
using ReSubMatch = srell::sub_match<std::string_view::const_iterator>;

// Re ctor flags (https://en.cppreference.com/cpp/regex/syntax_option_type):
// * Re::multiline
// * Re::dotall (SRELL's extension)
// * Re::icase
// * Re::nosubs
// * Re::optimize

// Reference: https://en.cppreference.com/cpp/regex/match_flag_type
enum class ReMatchFlags : u32
{
    DEFAULT = 0,
    MATCH_NOT_BOL = srell::regex_constants::match_flag_type::match_not_bol,
    MATCH_NOT_EOL = srell::regex_constants::match_flag_type::match_not_eol,
    MATCH_NOT_BOW = srell::regex_constants::match_flag_type::match_not_bow,
    MATCH_NOT_EOW = srell::regex_constants::match_flag_type::match_not_eow,
    MATCH_NOT_NULL = srell::regex_constants::match_flag_type::match_not_null,
    MATCH_CONTINUOUS = srell::regex_constants::match_flag_type::match_continuous,
    MATCH_PREV_AVAIL = srell::regex_constants::match_flag_type::match_prev_avail,
};
VSL_DECLARE_ENUM_FLAGS(ReMatchFlags)

// Reference: https://en.cppreference.com/cpp/regex/match_flag_type
enum class ReReplFlags : u32
{
    DEFAULT = 0,
    MATCH_NOT_BOL = srell::regex_constants::match_flag_type::match_not_bol,
    MATCH_NOT_EOL = srell::regex_constants::match_flag_type::match_not_eol,
    MATCH_NOT_BOW = srell::regex_constants::match_flag_type::match_not_bow,
    MATCH_NOT_EOW = srell::regex_constants::match_flag_type::match_not_eow,
    MATCH_NOT_NULL = srell::regex_constants::match_flag_type::match_not_null,
    MATCH_CONTINUOUS = srell::regex_constants::match_flag_type::match_continuous,
    MATCH_PREV_AVAIL = srell::regex_constants::match_flag_type::match_prev_avail,
    FORMAT_NO_COPY = srell::regex_constants::match_flag_type::format_no_copy,
    FORMAT_FIRST_ONLY = srell::regex_constants::match_flag_type::format_first_only,
};
VSL_DECLARE_ENUM_FLAGS(ReReplFlags)

enum class ReSplitOptions : u32
{
    NONE = 0,
    TRIM = 1 << 0,
    SKIP_EMPTY = 1 << 1,
};
VSL_DECLARE_ENUM_FLAGS(ReSplitOptions)

namespace detail
{

template<typename T>
concept regex_type = vsl::one_of_type<T, Re, ReAscii>;

template<typename T>
concept repl_type = vsl::one_of_type<T, std::string, const char*, char*>;

[[nodiscard]]
inline auto reserve_string_for(std::string_view s) -> std::string
{
    constexpr auto RESERVE_PART = 5;  // +20%
    const auto reserve_size = s.size() + (s.size() / RESERVE_PART);

    auto result_string = std::string{};
    const auto RESERVE_THRESHOLD = 2 * result_string.capacity();
    if (reserve_size > RESERVE_THRESHOLD)
    {
        result_string.reserve(reserve_size);
    }
    return result_string;
}

template<typename ReFlags>
    requires vsl::one_of_type<ReFlags, ReMatchFlags, ReReplFlags>
[[nodiscard]]
inline auto to_srell_flags(ReFlags flags) -> srell::regex_constants::match_flag_type
{
    return static_cast<srell::regex_constants::match_flag_type>(flags);
}

}  // namespace detail

[[nodiscard]]
inline auto sv(const ReMatch& m) noexcept -> std::string_view
{
    return std::string_view(m[0].first, m[0].second);
}

[[nodiscard]]
inline auto sv(const ReSubMatch& m) noexcept -> std::string_view
{
    return std::string_view(m.first, m.second);
}

template<detail::regex_type R>
[[nodiscard]]
inline auto re_full_match(std::string_view s, const R& re, ReMatchFlags flags = ReMatchFlags::DEFAULT) -> bool
{
    return srell::regex_match(s.begin(), s.end(), re, detail::to_srell_flags(flags));
}

template<detail::regex_type R>
inline auto re_full_match(std::string_view s, const R& re, ReMatch& match, ReMatchFlags flags = ReMatchFlags::DEFAULT)
    -> bool
{
    return srell::regex_match(s.begin(), s.end(), match, re, detail::to_srell_flags(flags));
}

template<detail::regex_type R>
[[nodiscard]]
inline auto re_search(std::string_view s, const R& re, ReMatchFlags flags = ReMatchFlags::DEFAULT) -> bool
{
    return srell::regex_search(s.begin(), s.end(), re, detail::to_srell_flags(flags));
}

template<detail::regex_type R>
inline auto re_search(std::string_view s, const R& re, ReMatch& match, ReMatchFlags flags = ReMatchFlags::DEFAULT)
    -> bool
{
    return srell::regex_search(s.begin(), s.end(), match, re, detail::to_srell_flags(flags));
}

// ---------------------------------------------------------------------------------------
// WARNING!
// Ensure that the regex object (re) passed to the function outlives the returned range
// ---------------------------------------------------------------------------------------
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
[[nodiscard]]
inline auto re_find_matches(std::string_view s, R&& re, ReMatchFlags flags = ReMatchFlags::DEFAULT)
    -> vsl::range_view_of<ReMatch> auto
{
    // TODO: Use srell::regex_iterator2

    auto matches_begin = srell::regex_iterator(s.begin(), s.end(), re, detail::to_srell_flags(flags));
    auto matches_end = decltype(matches_begin){};
    return std::ranges::subrange(std::move(matches_begin), std::move(matches_end));
}

// ---------------------------------------------------------------------------------------
// WARNING!
// Ensure that the regex object (re) passed to the function outlives the returned range
// ---------------------------------------------------------------------------------------
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
[[nodiscard]]
inline auto re_find_matches_sv(std::string_view s, R&& re, ReMatchFlags flags = ReMatchFlags::DEFAULT)
    -> vsl::range_view_of<std::string_view> auto
{
    return re_find_matches(s, re, flags) | std::views::transform([](const ReMatch& m) { return sv(m); });
}

namespace detail
{

template<regex_type R, typename Submatches>
    requires std::is_lvalue_reference_v<R&&>
[[nodiscard]]
inline auto re_find_submatches_impl(std::string_view s, R&& re, Submatches&& submatches, ReMatchFlags flags)
    -> vsl::range_view_of<std::string_view> auto
{
    auto matches_begin = srell::regex_token_iterator(s.begin(), s.end(), re, std::forward<Submatches>(submatches),
                                                     detail::to_srell_flags(flags));
    auto matches_end = decltype(matches_begin){};
    return std::ranges::subrange(std::move(matches_begin), std::move(matches_end))
           | std::views::transform([](const ReSubMatch& sm) { return sv(sm); });
}

}  // namespace detail

// ---------------------------------------------------------------------------------------
// WARNING!
// Ensure that the regex object (re) passed to the function outlives the returned range
// ---------------------------------------------------------------------------------------
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
[[nodiscard]]
inline auto re_find_submatches(std::string_view s, R&& re, int submatches, ReMatchFlags flags = ReMatchFlags::DEFAULT)
    -> vsl::range_view_of<std::string_view> auto
{
    return detail::re_find_submatches_impl(s, re, submatches, flags);
}

// ---------------------------------------------------------------------------------------
// WARNING!
// Ensure that the regex object (re) passed to the function outlives the returned range
// ---------------------------------------------------------------------------------------
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
[[nodiscard]]
inline auto re_find_submatches(std::string_view s,
                               R&& re,
                               const std::vector<int>& submatches,
                               ReMatchFlags flags = ReMatchFlags::DEFAULT) -> vsl::range_view_of<std::string_view> auto
{
    return detail::re_find_submatches_impl(s, re, submatches, flags);
}

// ---------------------------------------------------------------------------------------
// WARNING!
// Ensure that the regex object (re) passed to the function outlives the returned range
// ---------------------------------------------------------------------------------------
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
[[nodiscard]]
inline auto re_find_submatches(std::string_view s,
                               R&& re,
                               std::initializer_list<int> submatches,
                               ReMatchFlags flags = ReMatchFlags::DEFAULT) -> vsl::range_view_of<std::string_view> auto
{
    return detail::re_find_submatches_impl(s, re, submatches, flags);
}

// ---------------------------------------------------------------------------------------
// WARNING!
// Ensure that the regex object (re) passed to the function outlives the returned range
// ---------------------------------------------------------------------------------------
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
[[nodiscard]]
inline auto re_split(std::string_view s,
                     R&& re,
                     ReSplitOptions opt = ReSplitOptions::NONE,
                     ReMatchFlags flags = ReMatchFlags::DEFAULT) -> vsl::range_view_of<std::string_view> auto
{
    const auto trim_tokens = vsl::enum_contains_flags(opt, ReSplitOptions::TRIM);
    const auto skip_empty = vsl::enum_contains_flags(opt, ReSplitOptions::SKIP_EMPTY);
    return re_find_submatches(s, re, -1, flags)
           | std::views::transform([trim_tokens](std::string_view sv) noexcept
                                   { return trim_tokens ? vsl::trim(sv) : sv; })
           | std::views::filter([skip_empty](std::string_view sv) noexcept { return !(skip_empty && sv.empty()); });
}

template<typename OutputIt, typename BidirIt, detail::regex_type R, detail::repl_type Repl>
[[nodiscard]]
inline auto re_replace(
    OutputIt out, BidirIt first, BidirIt last, const R& re, const Repl& repl, ReReplFlags flags = ReReplFlags::DEFAULT)
    -> OutputIt
{
    return srell::regex_replace(out, first, last, re, repl, detail::to_srell_flags(flags));
}

template<detail::regex_type R, detail::repl_type Repl>
inline auto re_replace(
    std::string& out, std::string_view s, const R& re, const Repl& repl, ReReplFlags flags = ReReplFlags::DEFAULT)
    -> void
{
    srell::regex_replace(std::back_inserter(out), s.begin(), s.end(), re, repl, detail::to_srell_flags(flags));
}

template<detail::regex_type R, detail::repl_type Repl>
[[nodiscard]]
inline auto re_replace(std::string_view s, const R& re, const Repl& repl, ReReplFlags flags = ReReplFlags::DEFAULT)
    -> std::string
{
    auto res = detail::reserve_string_for(s);
    re_replace(res, s, re, repl, flags);
    return res;
}

template<detail::regex_type R>
inline auto re_replace(std::string& out,
                       std::string_view s,
                       const R& re,
                       std::function<std::string(const ReMatch&)> repl_func,
                       ReReplFlags flags = ReReplFlags::DEFAULT) -> void
{
    const auto first_only = vsl::enum_contains_flags(flags, ReReplFlags::FORMAT_FIRST_ONLY);
    const auto copy_unmatched = !vsl::enum_contains_flags(flags, ReReplFlags::FORMAT_NO_COPY);

    auto last_pos = ReMatch::size_type{0};

    const auto matches = re_find_matches(s, re, static_cast<ReMatchFlags>(flags));
    for (auto&& match : matches)
    {
        if (copy_unmatched)
        {
            out.append(match.prefix().first, match.prefix().second);
        }
        out.append(repl_func(match));
        last_pos = match.position() + match.length();
        if (first_only) break;
    }

    if (copy_unmatched)
    {
        out.append(s, last_pos, s.length() - last_pos);
    }
}

template<detail::regex_type R>
[[nodiscard]]
inline auto re_replace(std::string_view s,
                       const R& re,
                       std::function<std::string(const ReMatch&)> repl_func,
                       ReReplFlags flags = ReReplFlags::DEFAULT) -> std::string
{
    auto res = detail::reserve_string_for(s);
    re_replace(res, s, re, repl_func, flags);
    return res;
}

inline auto re_escape(std::string& out, std::string_view s) -> void
{
    for (char c : s)
    {
        switch (c)
        {
        case '.':
        case '^':
        case '$':
        case '*':
        case '+':
        case '?':
        case '|':
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case '\\':
            out += '\\';
            break;
        }
        out += c;
    }
}

[[nodiscard]]
inline auto re_escape(std::string_view s) -> std::string
{
    auto res = detail::reserve_string_for(s);
    re_escape(res, s);
    return res;
}

inline auto re_escape_repl(std::string& out, std::string_view s) -> void
{
    for (char c : s)
    {
        switch (c)
        {
        case '$':
            out += '$';
            break;
        }
        out += c;
    }
}

[[nodiscard]]
inline auto re_escape_repl(std::string_view s) -> std::string
{
    auto res = detail::reserve_string_for(s);
    re_escape_repl(res, s);
    return res;
}

}  // namespace vsl

#endif  // VSL_REGEX_H
