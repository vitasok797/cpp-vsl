#ifndef VSL_ENCODING_H
#define VSL_ENCODING_H

#include <vsl/encodings/all.h>
#include <vsl/encodings/detail.h>
#include <vsl/types.h>

#include <uni_algo/ranges_conv.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <string>
#include <string_view>

namespace vsl
{

namespace detail
{

inline constexpr unsigned char FIRST_NON_ASCII_CHAR = 0x80;

inline constexpr auto is_ascii_char(char c) noexcept -> bool
{
    return static_cast<unsigned char>(c) < FIRST_NON_ASCII_CHAR;
}

inline auto extract_leading_ascii(std::string& out, std::string_view str) -> std::string_view
{
    const auto first_non_ascii = std::ranges::find_if_not(str, is_ascii_char);
    const auto count = vsl::as_unsigned(std::distance(str.begin(), first_non_ascii));
    if (count > 0)
    {
        out.append(str.data(), count);
        str.remove_prefix(count);
    }
    return str;
}

template<size_t N>
inline auto append_replacement(std::string& out, std::array<unsigned char, N> repl) -> void
{
    assert(repl[0] != '\0');
    out += static_cast<char>(repl[0]);

    if constexpr (N > 1)
    {
        for (size_t i = 1; i < N; ++i)
        {
            if (repl[i] == '\0') return;
            out += static_cast<char>(repl[i]);
        }
    }
}

}  // namespace detail

[[nodiscard]]
inline constexpr auto is_ascii(std::string_view str) noexcept -> bool
{
    return std::ranges::all_of(str, detail::is_ascii_char);
}

template<encodings::detail::encoding_to Encoding>
inline auto to_encoding(std::string& out, std::string_view str, const Encoding&, char repl_char = '?') -> void
{
    str = detail::extract_leading_ascii(out, str);
    if (str.empty()) return;

    for (const u32 cp : una::ranges::utf8_view(str))
    {
        if (cp < detail::FIRST_NON_ASCII_CHAR)
        {
            out += static_cast<char>(cp);
            continue;
        }

        using MapPairType = decltype(Encoding::to_encoding_map)::value_type;
        const auto found_it = std::ranges::lower_bound(Encoding::to_encoding_map, cp, {}, &MapPairType::first);
        if (found_it != Encoding::to_encoding_map.end() && found_it->first == cp)
        {
            detail::append_replacement(out, found_it->second);
            continue;
        }

        out += repl_char;
    }
}

template<encodings::detail::encoding_to Encoding>
[[nodiscard]]
inline auto to_encoding(std::string_view str, const Encoding& encoding, char repl_char = '?') -> std::string
{
    auto res = std::string{};
    encoding.to_encoding_reserve(res, str);

    to_encoding(res, str, encoding, repl_char);

    return res;
}

template<encodings::detail::encoding_from Encoding>
inline auto from_encoding(std::string& out, std::string_view str, const Encoding&) -> void
{
    str = detail::extract_leading_ascii(out, str);
    if (str.empty()) return;

    for (const char c : str)
    {
        if (detail::is_ascii_char(c))
        {
            out += c;
        }
        else
        {
            const auto repl_index = static_cast<unsigned char>(c) - detail::FIRST_NON_ASCII_CHAR;
            detail::append_replacement(out, Encoding::from_encoding_map[repl_index]);
        }
    }
}

template<encodings::detail::encoding_from Encoding>
[[nodiscard]]
inline auto from_encoding(std::string_view str, const Encoding& encoding) -> std::string
{
    auto res = std::string{};
    encoding.from_encoding_reserve(res, str);

    from_encoding(res, str, encoding);

    return res;
}

}  // namespace vsl

#endif  // VSL_ENCODING_H
