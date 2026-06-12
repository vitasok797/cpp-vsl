#ifndef VSL_ENCODING_H
#define VSL_ENCODING_H

#include <vsl/encoding_charsets.h>
#include <vsl/types.h>

#include <uni_algo/conv.h>
#include <uni_algo/ranges_conv.h>

#include <string>
#include <string_view>

namespace vsl
{

namespace detail
{

inline constexpr auto LEAST_NONASCII_SYMBOL = u32{0x80};

}  // namespace detail

inline auto is_valid_utf8(std::string_view str) -> bool
{
    return una::is_valid_utf8<char>(str);
}

inline auto is_valid_utf16(std::u16string_view str) -> bool
{
    return una::is_valid_utf16<char16_t>(str);
}

inline auto is_ascii(std::string_view str) -> bool
{
    for (char c : str)
    {
        if (static_cast<unsigned char>(c) >= 128) return false;
    }
    return true;
}

inline auto to_utf16(std::string_view str) -> std::u16string
{
    return una::utf8to16<char, char16_t>(str);
}

inline auto from_utf16(std::u16string_view str) -> std::string
{
    return una::utf16to8<char16_t, char>(str);
}

template<typename Charset>
auto to_charset(std::string_view str, char repl_char = '?') -> std::string
{
    auto res = std::string{};
    res.reserve(str.size());

    for (u32 cp : una::ranges::utf8_view(str))
    {
        if (cp < detail::LEAST_NONASCII_SYMBOL)
        {
            res.push_back(static_cast<char>(cp));
        }
        else if (auto it = Charset::to_charset_map.find(cp); it != Charset::to_charset_map.end())
        {
            res.append(it->second);
        }
        else
        {
            res.push_back(repl_char);
        }
    }

    return res;
}

template<typename Charset>
auto from_charset(std::string_view str) -> std::string
{
    auto res = std::string{};
    res.reserve(str.size() * 3);

    for (char c : str)
    {
        const auto byte = static_cast<u8>(c);
        if (byte < detail::LEAST_NONASCII_SYMBOL)
        {
            res.push_back(c);
        }
        else
        {
            res.append(Charset::from_charset_map[byte - detail::LEAST_NONASCII_SYMBOL]);
        }
    }

    return res;
}

}  // namespace vsl

#endif  // VSL_ENCODING_H
