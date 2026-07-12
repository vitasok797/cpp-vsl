#ifndef VSL_ENCODINGS_CP1251_H
#define VSL_ENCODINGS_CP1251_H

#include <vsl/encodings/detail.h>

#include <array>
#include <string>
#include <string_view>

namespace vsl::encodings
{

namespace detail
{

struct cp1251_t
{
    static inline constexpr std::array<ToEncodingMapItem, 127> to_encoding_map = std::to_array<ToEncodingMapItem>({
        {0x00A0, "\xA0"},
        {0x00A4, "\xA4"},
        {0x00A6, "\xA6"},
        {0x00A7, "\xA7"},
        {0x00A9, "\xA9"},
        {0x00AB, "\xAB"},
        {0x00AC, "\xAC"},
        {0x00AD, "\xAD"},
        {0x00AE, "\xAE"},
        {0x00B0, "\xB0"},
        {0x00B1, "\xB1"},
        {0x00B5, "\xB5"},
        {0x00B6, "\xB6"},
        {0x00B7, "\xB7"},
        {0x00BB, "\xBB"},
        {0x0401, "\xA8"},
        {0x0402, "\x80"},
        {0x0403, "\x81"},
        {0x0404, "\xAA"},
        {0x0405, "\xBD"},
        {0x0406, "\xB2"},
        {0x0407, "\xAF"},
        {0x0408, "\xA3"},
        {0x0409, "\x8A"},
        {0x040A, "\x8C"},
        {0x040B, "\x8E"},
        {0x040C, "\x8D"},
        {0x040E, "\xA1"},
        {0x040F, "\x8F"},
        {0x0410, "\xC0"},
        {0x0411, "\xC1"},
        {0x0412, "\xC2"},
        {0x0413, "\xC3"},
        {0x0414, "\xC4"},
        {0x0415, "\xC5"},
        {0x0416, "\xC6"},
        {0x0417, "\xC7"},
        {0x0418, "\xC8"},
        {0x0419, "\xC9"},
        {0x041A, "\xCA"},
        {0x041B, "\xCB"},
        {0x041C, "\xCC"},
        {0x041D, "\xCD"},
        {0x041E, "\xCE"},
        {0x041F, "\xCF"},
        {0x0420, "\xD0"},
        {0x0421, "\xD1"},
        {0x0422, "\xD2"},
        {0x0423, "\xD3"},
        {0x0424, "\xD4"},
        {0x0425, "\xD5"},
        {0x0426, "\xD6"},
        {0x0427, "\xD7"},
        {0x0428, "\xD8"},
        {0x0429, "\xD9"},
        {0x042A, "\xDA"},
        {0x042B, "\xDB"},
        {0x042C, "\xDC"},
        {0x042D, "\xDD"},
        {0x042E, "\xDE"},
        {0x042F, "\xDF"},
        {0x0430, "\xE0"},
        {0x0431, "\xE1"},
        {0x0432, "\xE2"},
        {0x0433, "\xE3"},
        {0x0434, "\xE4"},
        {0x0435, "\xE5"},
        {0x0436, "\xE6"},
        {0x0437, "\xE7"},
        {0x0438, "\xE8"},
        {0x0439, "\xE9"},
        {0x043A, "\xEA"},
        {0x043B, "\xEB"},
        {0x043C, "\xEC"},
        {0x043D, "\xED"},
        {0x043E, "\xEE"},
        {0x043F, "\xEF"},
        {0x0440, "\xF0"},
        {0x0441, "\xF1"},
        {0x0442, "\xF2"},
        {0x0443, "\xF3"},
        {0x0444, "\xF4"},
        {0x0445, "\xF5"},
        {0x0446, "\xF6"},
        {0x0447, "\xF7"},
        {0x0448, "\xF8"},
        {0x0449, "\xF9"},
        {0x044A, "\xFA"},
        {0x044B, "\xFB"},
        {0x044C, "\xFC"},
        {0x044D, "\xFD"},
        {0x044E, "\xFE"},
        {0x044F, "\xFF"},
        {0x0451, "\xB8"},
        {0x0452, "\x90"},
        {0x0453, "\x83"},
        {0x0454, "\xBA"},
        {0x0455, "\xBE"},
        {0x0456, "\xB3"},
        {0x0457, "\xBF"},
        {0x0458, "\xBC"},
        {0x0459, "\x9A"},
        {0x045A, "\x9C"},
        {0x045B, "\x9E"},
        {0x045C, "\x9D"},
        {0x045E, "\xA2"},
        {0x045F, "\x9F"},
        {0x0490, "\xA5"},
        {0x0491, "\xB4"},
        {0x2013, "\x96"},
        {0x2014, "\x97"},
        {0x2018, "\x91"},
        {0x2019, "\x92"},
        {0x201A, "\x82"},
        {0x201C, "\x93"},
        {0x201D, "\x94"},
        {0x201E, "\x84"},
        {0x2020, "\x86"},
        {0x2021, "\x87"},
        {0x2022, "\x95"},
        {0x2026, "\x85"},
        {0x2030, "\x89"},
        {0x2039, "\x8B"},
        {0x203A, "\x9B"},
        {0x20AC, "\x88"},
        {0x2116, "\xB9"},
        {0x2122, "\x99"},
    });

    static inline constexpr FromEncodingMap from_encoding_map{
        "\xD0\x82",     "\xD0\x83",     "\xE2\x80\x9A", "\xD1\x93",     "\xE2\x80\x9E", "\xE2\x80\xA6", "\xE2\x80\xA0",
        "\xE2\x80\xA1", "\xE2\x82\xAC", "\xE2\x80\xB0", "\xD0\x89",     "\xE2\x80\xB9", "\xD0\x8A",     "\xD0\x8C",
        "\xD0\x8B",     "\xD0\x8F",     "\xD1\x92",     "\xE2\x80\x98", "\xE2\x80\x99", "\xE2\x80\x9C", "\xE2\x80\x9D",
        "\xE2\x80\xA2", "\xE2\x80\x93", "\xE2\x80\x94", UTF8_REPL_CHAR, "\xE2\x84\xA2", "\xD1\x99",     "\xE2\x80\xBA",
        "\xD1\x9A",     "\xD1\x9C",     "\xD1\x9B",     "\xD1\x9F",     "\xC2\xA0",     "\xD0\x8E",     "\xD1\x9E",
        "\xD0\x88",     "\xC2\xA4",     "\xD2\x90",     "\xC2\xA6",     "\xC2\xA7",     "\xD0\x81",     "\xC2\xA9",
        "\xD0\x84",     "\xC2\xAB",     "\xC2\xAC",     "\xC2\xAD",     "\xC2\xAE",     "\xD0\x87",     "\xC2\xB0",
        "\xC2\xB1",     "\xD0\x86",     "\xD1\x96",     "\xD2\x91",     "\xC2\xB5",     "\xC2\xB6",     "\xC2\xB7",
        "\xD1\x91",     "\xE2\x84\x96", "\xD1\x94",     "\xC2\xBB",     "\xD1\x98",     "\xD0\x85",     "\xD1\x95",
        "\xD1\x97",     "\xD0\x90",     "\xD0\x91",     "\xD0\x92",     "\xD0\x93",     "\xD0\x94",     "\xD0\x95",
        "\xD0\x96",     "\xD0\x97",     "\xD0\x98",     "\xD0\x99",     "\xD0\x9A",     "\xD0\x9B",     "\xD0\x9C",
        "\xD0\x9D",     "\xD0\x9E",     "\xD0\x9F",     "\xD0\xA0",     "\xD0\xA1",     "\xD0\xA2",     "\xD0\xA3",
        "\xD0\xA4",     "\xD0\xA5",     "\xD0\xA6",     "\xD0\xA7",     "\xD0\xA8",     "\xD0\xA9",     "\xD0\xAA",
        "\xD0\xAB",     "\xD0\xAC",     "\xD0\xAD",     "\xD0\xAE",     "\xD0\xAF",     "\xD0\xB0",     "\xD0\xB1",
        "\xD0\xB2",     "\xD0\xB3",     "\xD0\xB4",     "\xD0\xB5",     "\xD0\xB6",     "\xD0\xB7",     "\xD0\xB8",
        "\xD0\xB9",     "\xD0\xBA",     "\xD0\xBB",     "\xD0\xBC",     "\xD0\xBD",     "\xD0\xBE",     "\xD0\xBF",
        "\xD1\x80",     "\xD1\x81",     "\xD1\x82",     "\xD1\x83",     "\xD1\x84",     "\xD1\x85",     "\xD1\x86",
        "\xD1\x87",     "\xD1\x88",     "\xD1\x89",     "\xD1\x8A",     "\xD1\x8B",     "\xD1\x8C",     "\xD1\x8D",
        "\xD1\x8E",     "\xD1\x8F",
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

inline constexpr auto cp1251 = detail::cp1251_t{};

}  // namespace vsl::encodings

#endif  // VSL_ENCODINGS_CP1251_H
