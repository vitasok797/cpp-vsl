#ifndef VSL_ENCODINGS_CP866_H
#define VSL_ENCODINGS_CP866_H

#include <vsl/encodings/detail.h>

#include <array>
#include <string>
#include <string_view>

namespace vsl::encodings
{

namespace detail
{

struct cp866_t
{
    static inline constexpr std::array<ToEncodingMapItem, 128> to_encoding_map = std::to_array<ToEncodingMapItem>({
        {0x00A0, "\xFF"},
        {0x00A4, "\xFD"},
        {0x00B0, "\xF8"},
        {0x00B7, "\xFA"},
        {0x0401, "\xF0"},
        {0x0404, "\xF2"},
        {0x0407, "\xF4"},
        {0x040E, "\xF6"},
        {0x0410, "\x80"},
        {0x0411, "\x81"},
        {0x0412, "\x82"},
        {0x0413, "\x83"},
        {0x0414, "\x84"},
        {0x0415, "\x85"},
        {0x0416, "\x86"},
        {0x0417, "\x87"},
        {0x0418, "\x88"},
        {0x0419, "\x89"},
        {0x041A, "\x8A"},
        {0x041B, "\x8B"},
        {0x041C, "\x8C"},
        {0x041D, "\x8D"},
        {0x041E, "\x8E"},
        {0x041F, "\x8F"},
        {0x0420, "\x90"},
        {0x0421, "\x91"},
        {0x0422, "\x92"},
        {0x0423, "\x93"},
        {0x0424, "\x94"},
        {0x0425, "\x95"},
        {0x0426, "\x96"},
        {0x0427, "\x97"},
        {0x0428, "\x98"},
        {0x0429, "\x99"},
        {0x042A, "\x9A"},
        {0x042B, "\x9B"},
        {0x042C, "\x9C"},
        {0x042D, "\x9D"},
        {0x042E, "\x9E"},
        {0x042F, "\x9F"},
        {0x0430, "\xA0"},
        {0x0431, "\xA1"},
        {0x0432, "\xA2"},
        {0x0433, "\xA3"},
        {0x0434, "\xA4"},
        {0x0435, "\xA5"},
        {0x0436, "\xA6"},
        {0x0437, "\xA7"},
        {0x0438, "\xA8"},
        {0x0439, "\xA9"},
        {0x043A, "\xAA"},
        {0x043B, "\xAB"},
        {0x043C, "\xAC"},
        {0x043D, "\xAD"},
        {0x043E, "\xAE"},
        {0x043F, "\xAF"},
        {0x0440, "\xE0"},
        {0x0441, "\xE1"},
        {0x0442, "\xE2"},
        {0x0443, "\xE3"},
        {0x0444, "\xE4"},
        {0x0445, "\xE5"},
        {0x0446, "\xE6"},
        {0x0447, "\xE7"},
        {0x0448, "\xE8"},
        {0x0449, "\xE9"},
        {0x044A, "\xEA"},
        {0x044B, "\xEB"},
        {0x044C, "\xEC"},
        {0x044D, "\xED"},
        {0x044E, "\xEE"},
        {0x044F, "\xEF"},
        {0x0451, "\xF1"},
        {0x0454, "\xF3"},
        {0x0457, "\xF5"},
        {0x045E, "\xF7"},
        {0x2116, "\xFC"},
        {0x2219, "\xF9"},
        {0x221A, "\xFB"},
        {0x2500, "\xC4"},
        {0x2502, "\xB3"},
        {0x250C, "\xDA"},
        {0x2510, "\xBF"},
        {0x2514, "\xC0"},
        {0x2518, "\xD9"},
        {0x251C, "\xC3"},
        {0x2524, "\xB4"},
        {0x252C, "\xC2"},
        {0x2534, "\xC1"},
        {0x253C, "\xC5"},
        {0x2550, "\xCD"},
        {0x2551, "\xBA"},
        {0x2552, "\xD5"},
        {0x2553, "\xD6"},
        {0x2554, "\xC9"},
        {0x2555, "\xB8"},
        {0x2556, "\xB7"},
        {0x2557, "\xBB"},
        {0x2558, "\xD4"},
        {0x2559, "\xD3"},
        {0x255A, "\xC8"},
        {0x255B, "\xBE"},
        {0x255C, "\xBD"},
        {0x255D, "\xBC"},
        {0x255E, "\xC6"},
        {0x255F, "\xC7"},
        {0x2560, "\xCC"},
        {0x2561, "\xB5"},
        {0x2562, "\xB6"},
        {0x2563, "\xB9"},
        {0x2564, "\xD1"},
        {0x2565, "\xD2"},
        {0x2566, "\xCB"},
        {0x2567, "\xCF"},
        {0x2568, "\xD0"},
        {0x2569, "\xCA"},
        {0x256A, "\xD8"},
        {0x256B, "\xD7"},
        {0x256C, "\xCE"},
        {0x2580, "\xDF"},
        {0x2584, "\xDC"},
        {0x2588, "\xDB"},
        {0x258C, "\xDD"},
        {0x2590, "\xDE"},
        {0x2591, "\xB0"},
        {0x2592, "\xB1"},
        {0x2593, "\xB2"},
        {0x25A0, "\xFE"},
    });

    static inline constexpr FromEncodingMap from_encoding_map{
        "\xD0\x90",     "\xD0\x91",     "\xD0\x92",     "\xD0\x93",     "\xD0\x94",     "\xD0\x95",     "\xD0\x96",
        "\xD0\x97",     "\xD0\x98",     "\xD0\x99",     "\xD0\x9A",     "\xD0\x9B",     "\xD0\x9C",     "\xD0\x9D",
        "\xD0\x9E",     "\xD0\x9F",     "\xD0\xA0",     "\xD0\xA1",     "\xD0\xA2",     "\xD0\xA3",     "\xD0\xA4",
        "\xD0\xA5",     "\xD0\xA6",     "\xD0\xA7",     "\xD0\xA8",     "\xD0\xA9",     "\xD0\xAA",     "\xD0\xAB",
        "\xD0\xAC",     "\xD0\xAD",     "\xD0\xAE",     "\xD0\xAF",     "\xD0\xB0",     "\xD0\xB1",     "\xD0\xB2",
        "\xD0\xB3",     "\xD0\xB4",     "\xD0\xB5",     "\xD0\xB6",     "\xD0\xB7",     "\xD0\xB8",     "\xD0\xB9",
        "\xD0\xBA",     "\xD0\xBB",     "\xD0\xBC",     "\xD0\xBD",     "\xD0\xBE",     "\xD0\xBF",     "\xE2\x96\x91",
        "\xE2\x96\x92", "\xE2\x96\x93", "\xE2\x94\x82", "\xE2\x94\xA4", "\xE2\x95\xA1", "\xE2\x95\xA2", "\xE2\x95\x96",
        "\xE2\x95\x95", "\xE2\x95\xA3", "\xE2\x95\x91", "\xE2\x95\x97", "\xE2\x95\x9D", "\xE2\x95\x9C", "\xE2\x95\x9B",
        "\xE2\x94\x90", "\xE2\x94\x94", "\xE2\x94\xB4", "\xE2\x94\xAC", "\xE2\x94\x9C", "\xE2\x94\x80", "\xE2\x94\xBC",
        "\xE2\x95\x9E", "\xE2\x95\x9F", "\xE2\x95\x9A", "\xE2\x95\x94", "\xE2\x95\xA9", "\xE2\x95\xA6", "\xE2\x95\xA0",
        "\xE2\x95\x90", "\xE2\x95\xAC", "\xE2\x95\xA7", "\xE2\x95\xA8", "\xE2\x95\xA4", "\xE2\x95\xA5", "\xE2\x95\x99",
        "\xE2\x95\x98", "\xE2\x95\x92", "\xE2\x95\x93", "\xE2\x95\xAB", "\xE2\x95\xAA", "\xE2\x94\x98", "\xE2\x94\x8C",
        "\xE2\x96\x88", "\xE2\x96\x84", "\xE2\x96\x8C", "\xE2\x96\x90", "\xE2\x96\x80", "\xD1\x80",     "\xD1\x81",
        "\xD1\x82",     "\xD1\x83",     "\xD1\x84",     "\xD1\x85",     "\xD1\x86",     "\xD1\x87",     "\xD1\x88",
        "\xD1\x89",     "\xD1\x8A",     "\xD1\x8B",     "\xD1\x8C",     "\xD1\x8D",     "\xD1\x8E",     "\xD1\x8F",
        "\xD0\x81",     "\xD1\x91",     "\xD0\x84",     "\xD1\x94",     "\xD0\x87",     "\xD1\x97",     "\xD0\x8E",
        "\xD1\x9E",     "\xC2\xB0",     "\xE2\x88\x99", "\xC2\xB7",     "\xE2\x88\x9A", "\xE2\x84\x96", "\xC2\xA4",
        "\xE2\x96\xA0", "\xC2\xA0",
    };

    auto to_encoding_reserve(std::string& out, std::string_view str) const -> void
    {
        out.reserve(str.size());
    }

    auto from_encoding_reserve(std::string& out, std::string_view str) const -> void
    {
        out.reserve(str.size() * 3);
    }
};

}  // namespace detail

inline constexpr auto cp866 = detail::cp866_t{};

}  // namespace vsl::encodings

#endif  // VSL_ENCODINGS_CP866_H
