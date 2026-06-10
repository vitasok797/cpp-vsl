#ifndef VSL_TEXT_TRIM_H
#define VSL_TEXT_TRIM_H

#include <string_view>

namespace vsl
{

namespace detail
{

inline constexpr auto WHITESPACES = std::string_view{" \n\r\t\f\v"};
inline constexpr auto NBSP = std::string_view{"\xC2\xA0"};

}  // namespace detail

inline constexpr auto trim_left(std::string_view str, std::string_view symbols) noexcept -> std::string_view
{
    const auto first = str.find_first_not_of(symbols);
    if (first == std::string_view::npos) return {};
    str.remove_prefix(first);
    return str;
}

inline constexpr auto trim_right(std::string_view str, std::string_view symbols) noexcept -> std::string_view
{
    const auto last = str.find_last_not_of(symbols);
    if (last == std::string_view::npos) return {};
    str.remove_suffix(str.size() - last - 1);
    return str;
}

inline constexpr auto trim_left(std::string_view str) noexcept -> std::string_view
{
    while (true)
    {
        str = trim_left(str, detail::WHITESPACES);
        if (!str.starts_with(detail::NBSP)) return str;
        str.remove_prefix(detail::NBSP.size());
    }
}

inline constexpr auto trim_right(std::string_view str) noexcept -> std::string_view
{
    while (true)
    {
        str = trim_right(str, detail::WHITESPACES);
        if (!str.ends_with(detail::NBSP)) return str;
        str.remove_suffix(detail::NBSP.size());
    }
}

inline constexpr auto trim(std::string_view s, std::string_view symbols) noexcept -> std::string_view
{
    return trim_right(trim_left(s, symbols), symbols);
}

inline constexpr auto trim(std::string_view s) noexcept -> std::string_view
{
    return trim_right(trim_left(s));
}

}  // namespace vsl

#endif  // VSL_TEXT_TRIM_H
