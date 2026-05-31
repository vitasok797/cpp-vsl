#ifndef VSL_REGEX_H
#define VSL_REGEX_H

#include <vsl/concepts.h>
#include <vsl/enum.h>
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

// Re ctor flags (https://en.cppreference.com/cpp/regex/syntax_option_type):
// * Re::multiline
// * Re::dotall (SRELL's extension)
// * Re::icase
// * Re::nosubs
// * Re::optimize

// re_* functions flags (https://en.cppreference.com/cpp/regex/match_flag_type):
// * re_const::match_default
// * re_const::match_not_bol
// * re_const::match_not_eol
// * re_const::match_not_bow
// * re_const::match_not_eow
// * re_const::match_any
// * re_const::match_not_null
// * re_const::match_continuous
// * re_const::match_prev_avail
// * re_const::format_default
// * re_const::format_sed
// * re_const::format_no_copy
// * re_const::format_first_only

namespace re_const = srell::regex_constants;

using ReMatchFlagType = srell::regex_constants::match_flag_type;

using ReError = srell::regex_error;

using Re = srell::u8cregex;
using ReAscii = srell::regex;

using ReMatch = srell::match_results<std::string_view::const_iterator>;
using ReSubMatch = srell::sub_match<std::string_view::const_iterator>;

namespace detail
{

template<typename T>
concept regex_type = vsl::one_of<T, Re, ReAscii>;

template<typename T>
concept repl_type = vsl::one_of<T, std::string, const char*, char*>;

inline auto create_result_string_for(std::string_view s) -> std::string
{
    auto result_string = std::string{};

    constexpr auto LARGE_STRING_THRESHOLD = std::string_view::size_type{64};

    if (s.size() > LARGE_STRING_THRESHOLD)
    {
        constexpr auto RESERVE_PART = 5;  // +20%
        result_string.reserve(s.size() + (s.size() / RESERVE_PART));
    }
    else
    {
        result_string.reserve(s.size());
    }

    return result_string;
}

}  // namespace detail

inline auto sv(const ReMatch& m) noexcept -> std::string_view
{
    return std::string_view(m[0].first, m[0].second);
}

inline auto sv(const ReSubMatch& m) noexcept -> std::string_view
{
    return std::string_view(m.first, m.second);
}

template<detail::regex_type R>
auto re_full_match(std::string_view s, const R& re, ReMatchFlagType flags = re_const::match_default) -> bool
{
    return srell::regex_match(s.begin(), s.end(), re, flags);
}

template<detail::regex_type R>
auto re_full_match(std::string_view s, const R& re, ReMatch& match, ReMatchFlagType flags = re_const::match_default)
    -> bool
{
    return srell::regex_match(s.begin(), s.end(), match, re, flags);
}

template<detail::regex_type R>
auto re_search(std::string_view s, const R& re, ReMatchFlagType flags = re_const::match_default) -> bool
{
    return srell::regex_search(s.begin(), s.end(), re, flags);
}

template<detail::regex_type R>
auto re_search(std::string_view s, const R& re, ReMatch& match, ReMatchFlagType flags = re_const::match_default) -> bool
{
    return srell::regex_search(s.begin(), s.end(), match, re, flags);
}

// Ensure that the regex object (re) passed to the function outlives the returned range
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
auto re_find_matches(std::string_view s, R&& re, ReMatchFlagType flags = re_const::match_default) -> auto
{
    auto matches_begin = srell::regex_iterator(s.begin(), s.end(), re, flags);
    auto matches_end = decltype(matches_begin){};
    return std::ranges::subrange(std::move(matches_begin), std::move(matches_end));
}

// Ensure that the regex object (re) passed to the function outlives the returned range
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
auto re_find_matches_sv(std::string_view s, R&& re, ReMatchFlagType flags = re_const::match_default) -> auto
{
    return re_find_matches(s, re, flags) | std::views::transform([](const ReMatch& m) { return sv(m); });
}

namespace detail
{

template<regex_type R, typename Submatches>
    requires std::is_lvalue_reference_v<R&&>
auto re_find_submatches_impl(std::string_view s, R&& re, Submatches&& submatches, ReMatchFlagType flags) -> auto
{
    auto matches_begin =
        srell::regex_token_iterator(s.begin(), s.end(), re, std::forward<Submatches>(submatches), flags);
    auto matches_end = decltype(matches_begin){};
    return std::ranges::subrange(std::move(matches_begin), std::move(matches_end))
           | std::views::transform([](const ReSubMatch& sm) { return sv(sm); });
}

}  // namespace detail

// Ensure that the regex object (re) passed to the function outlives the returned range
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
auto re_find_submatches(std::string_view s, R&& re, int submatches, ReMatchFlagType flags = re_const::match_default)
    -> auto
{
    return detail::re_find_submatches_impl(s, re, submatches, flags);
}

// Ensure that the regex object (re) passed to the function outlives the returned range
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
auto re_find_submatches(std::string_view s,
                        R&& re,
                        const std::vector<int>& submatches,
                        ReMatchFlagType flags = re_const::match_default) -> auto
{
    return detail::re_find_submatches_impl(s, re, submatches, flags);
}

// Ensure that the regex object (re) passed to the function outlives the returned range
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
auto re_find_submatches(std::string_view s,
                        R&& re,
                        std::initializer_list<int> submatches,
                        ReMatchFlagType flags = re_const::match_default) -> auto
{
    return detail::re_find_submatches_impl(s, re, submatches, flags);
}

namespace detail
{

inline constexpr auto WHITESPACES = " \n\r\t\f\v";

inline constexpr auto trim(std::string_view s) noexcept -> std::string_view
{
    const auto first = s.find_first_not_of(WHITESPACES);
    if (first == std::string_view::npos) return {};

    const auto last = s.find_last_not_of(WHITESPACES);
    const auto count = last - first + 1;

    return s.substr(first, count);
}

}  // namespace detail

enum class ReSplitOptions : u32
{
    NONE = 0,
    TRIM = u32{1} << 0,
    SKIP_EMPTY = u32{1} << 1,
};
VSL_DECLARE_ENUM_FLAGS(ReSplitOptions)

// Ensure that the regex object (re) passed to the function outlives the returned range
template<detail::regex_type R>
    requires std::is_lvalue_reference_v<R&&>
auto re_split(std::string_view s,
              R&& re,
              ReSplitOptions opt = ReSplitOptions::NONE,
              ReMatchFlagType flags = re_const::match_default) -> auto
{
    const auto trim_tokens = vsl::enum_contains_flags(opt, ReSplitOptions::TRIM);
    const auto skip_empty = vsl::enum_contains_flags(opt, ReSplitOptions::SKIP_EMPTY);
    return re_find_submatches(s, re, -1, flags)
           | std::views::transform([trim_tokens](std::string_view sv) noexcept
                                   { return trim_tokens ? detail::trim(sv) : sv; })
           | std::views::filter([skip_empty](std::string_view sv) noexcept { return !(skip_empty && sv.empty()); });
}

template<typename OutputIt, typename BidirIt, detail::regex_type R, detail::repl_type Repl>
auto re_replace(OutputIt out,
                BidirIt first,
                BidirIt last,
                const R& re,
                const Repl& repl,
                ReMatchFlagType flags = re_const::match_default) -> OutputIt
{
    return srell::regex_replace(out, first, last, re, repl, flags);
}

template<detail::regex_type R, detail::repl_type Repl>
auto re_replace(std::string_view s, const R& re, const Repl& repl, ReMatchFlagType flags = re_const::match_default)
    -> std::string
{
    auto res = detail::create_result_string_for(s);
    srell::regex_replace(std::back_inserter(res), s.begin(), s.end(), re, repl, flags);
    return res;
}

template<detail::regex_type R>
auto re_replace(std::string_view s,
                const R& re,
                std::function<std::string(const ReMatch&)> repl_func,
                ReMatchFlagType flags = re_const::match_default) -> std::string
{
    const auto first_only = bool(flags & re_const::format_first_only);
    const auto copy_unmatched = !bool(flags & re_const::format_no_copy);

    auto res = detail::create_result_string_for(s);
    auto last_pos = ReMatch::size_type{0};

    const auto matches = re_find_matches(s, re, flags);
    for (auto&& match : matches)
    {
        if (copy_unmatched)
        {
            res.append(match.prefix().first, match.prefix().second);
        }
        res.append(repl_func(match));
        last_pos = match.position() + match.length();
        if (first_only) break;
    }

    if (copy_unmatched)
    {
        res.append(s, last_pos, s.length() - last_pos);
    }

    return res;
}

inline auto re_escape(std::string_view s) -> std::string
{
    auto res = detail::create_result_string_for(s);
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
            res += '\\';
            break;
        }
        res += c;
    }
    return res;
}

inline auto re_escape_repl(std::string_view s) -> std::string
{
    auto res = detail::create_result_string_for(s);
    for (char c : s)
    {
        switch (c)
        {
        case '$':
            res += '$';
            break;
        }
        res += c;
    }
    return res;
}

}  // namespace vsl

#endif  // VSL_REGEX_H
