#ifndef VSL_ENCODING_H
#define VSL_ENCODING_H

#include <vsl/encodings/all.h>
#include <vsl/encodings/detail.h>
#include <vsl/types.h>

#include <uni_algo/ranges_conv.h>

#include <algorithm>
#include <concepts>
#include <string>
#include <string_view>

namespace vsl
{

namespace detail
{

using vsl::encodings::detail::Codepoint;
using vsl::encodings::detail::ToEncodingMapItem;

template<typename ToEncodingMap>
consteval auto is_to_encoding_map_sorted(const ToEncodingMap& map) -> bool
{
    return std::ranges::is_sorted(map, std::less<>{}, &ToEncodingMapItem::first);
}

template<typename Encoding>
concept encodable = requires(const Encoding& encoding, std::string& s, std::string_view sv) {
    Encoding::to_encoding_map;
    requires Encoding::to_encoding_map.size() <= 128;
    requires is_to_encoding_map_sorted(Encoding::to_encoding_map);
    { encoding.to_encoding_reserve(s, sv) } -> std::same_as<void>;
};

template<typename Encoding>
concept decodable = requires(const Encoding& encoding, std::string& s, std::string_view sv) {
    Encoding::from_encoding_map;
    requires Encoding::from_encoding_map.size() == 128;
    { encoding.from_encoding_reserve(s, sv) } -> std::same_as<void>;
};

inline constexpr auto MIN_NON_ASCII_CODEPOINT = Codepoint{0x80};

}  // namespace detail

[[nodiscard]]
inline constexpr auto is_ascii(std::string_view str) noexcept -> bool
{
    auto is_ascii_char = [](unsigned char c) noexcept { return c < detail::MIN_NON_ASCII_CODEPOINT; };
    return std::ranges::all_of(str, is_ascii_char);
}

template<detail::encodable Encoding>
inline auto to_encoding(std::string& out, std::string_view str, const Encoding& encoding, char repl_char = '?') -> void
{
    if (is_ascii(str))
    {
        out.append(str);
        return;
    }

    for (detail::Codepoint cp : una::ranges::utf8_view(str))
    {
        if (cp < detail::MIN_NON_ASCII_CODEPOINT)
        {
            out.push_back(static_cast<char>(cp));
            continue;
        }

        auto map_record_it =
            std::ranges::lower_bound(encoding.to_encoding_map, cp, {}, &detail::ToEncodingMapItem::first);
        if (map_record_it != encoding.to_encoding_map.end() && map_record_it->first == cp)
        {
            out.append(map_record_it->second);
            continue;
        }

        out.push_back(repl_char);
    }
}

template<detail::encodable Encoding>
[[nodiscard]]
inline auto to_encoding(std::string_view str, const Encoding& encoding, char repl_char = '?') -> std::string
{
    auto res = std::string{};
    encoding.to_encoding_reserve(res, str);

    to_encoding(res, str, encoding, repl_char);

    return res;
}

template<detail::decodable Encoding>
inline auto from_encoding(std::string& out, std::string_view str, const Encoding& encoding) -> void
{
    for (char c : str)
    {
        const auto byte = static_cast<u8>(c);
        if (byte < detail::MIN_NON_ASCII_CODEPOINT)
        {
            out.push_back(c);
        }
        else
        {
            out.append(encoding.from_encoding_map[byte - detail::MIN_NON_ASCII_CODEPOINT]);
        }
    }
}

template<detail::decodable Encoding>
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
