#ifndef VSL_ENCODING_H
#define VSL_ENCODING_H

#include <vsl/types.h>

#include <uni_algo/conv.h>
#include <uni_algo/ranges_conv.h>

#include <array>
#include <string>
#include <string_view>
#include <unordered_map>

namespace vsl
{

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

inline auto utf8to16(std::string_view str) -> std::u16string
{
    return una::utf8to16<char, char16_t>(str);
}

inline auto utf16to8(std::u16string_view str) -> std::string
{
    return una::utf16to8<char16_t, char>(str);
}

namespace detail
{

using ToCharsetMap = std::unordered_map<u32, std::string_view>;
using FromCharsetMap = std::array<std::string_view, 128>;

inline constexpr auto LEAST_NONASCII_SYMBOL = u32{0x80};
inline constexpr auto UTF_REPL_CHAR = "\xEF\xBF\xBD";

inline const auto to_translit_map = ToCharsetMap{
    // ГОСТ 7.79-2000, Схема Б
    {0x0410,   "A"}, // А
    {0x0430,   "a"}, // а
    {0x0411,   "B"}, // Б
    {0x0431,   "b"}, // б
    {0x0412,   "V"}, // В
    {0x0432,   "v"}, // в
    {0x0413,   "G"}, // Г
    {0x0433,   "g"}, // г
    {0x0414,   "D"}, // Д
    {0x0434,   "d"}, // д
    {0x0415,   "E"}, // Е
    {0x0435,   "e"}, // е
    {0x0401,  "Yo"}, // Ё
    {0x0451,  "yo"}, // ё
    {0x0416,  "Zh"}, // Ж
    {0x0436,  "zh"}, // ж
    {0x0417,   "Z"}, // З
    {0x0437,   "z"}, // з
    {0x0418,   "I"}, // И
    {0x0438,   "i"}, // и
    {0x0419,   "J"}, // Й
    {0x0439,   "j"}, // й
    {0x041A,   "K"}, // К
    {0x043A,   "k"}, // к
    {0x041B,   "L"}, // Л
    {0x043B,   "l"}, // л
    {0x041C,   "M"}, // М
    {0x043C,   "m"}, // м
    {0x041D,   "N"}, // Н
    {0x043D,   "n"}, // н
    {0x041E,   "O"}, // О
    {0x043E,   "o"}, // о
    {0x041F,   "P"}, // П
    {0x043F,   "p"}, // п
    {0x0420,   "R"}, // Р
    {0x0440,   "r"}, // р
    {0x0421,   "S"}, // С
    {0x0441,   "s"}, // с
    {0x0422,   "T"}, // Т
    {0x0442,   "t"}, // т
    {0x0423,   "U"}, // У
    {0x0443,   "u"}, // у
    {0x0424,   "F"}, // Ф
    {0x0444,   "f"}, // ф
    {0x0425,   "X"}, // Х
    {0x0445,   "x"}, // х
    {0x0426,  "Cz"}, // Ц
    {0x0446,  "cz"}, // ц
    {0x0427,  "Ch"}, // Ч
    {0x0447,  "ch"}, // ч
    {0x0428,  "Sh"}, // Ш
    {0x0448,  "sh"}, // ш
    {0x0429, "Shh"}, // Щ
    {0x0449, "shh"}, // щ
    {0x042A,  "``"}, // Ъ
    {0x044A,  "``"}, // ъ
    {0x042B,  "Y`"}, // Ы
    {0x044B,  "y`"}, // ы
    {0x042C,   "`"}, // Ь
    {0x044C,   "`"}, // ь
    {0x042D,  "E`"}, // Э
    {0x044D,  "e`"}, // э
    {0x042E,  "Yu"}, // Ю
    {0x044E,  "yu"}, // ю
    {0x042F,  "Ya"}, // Я
    {0x044F,  "ya"}, // я
};

inline const auto to_cp1251_map = ToCharsetMap{
    {0x0402, "\x80"},
    {0x0403, "\x81"},
    {0x201A, "\x82"},
    {0x0453, "\x83"},
    {0x201E, "\x84"},
    {0x2026, "\x85"},
    {0x2020, "\x86"},
    {0x2021, "\x87"},
    {0x20AC, "\x88"},
    {0x2030, "\x89"},
    {0x0409, "\x8A"},
    {0x2039, "\x8B"},
    {0x040A, "\x8C"},
    {0x040C, "\x8D"},
    {0x040B, "\x8E"},
    {0x040F, "\x8F"},
    {0x0452, "\x90"},
    {0x2018, "\x91"},
    {0x2019, "\x92"},
    {0x201C, "\x93"},
    {0x201D, "\x94"},
    {0x2022, "\x95"},
    {0x2013, "\x96"},
    {0x2014, "\x97"},
    // no 0x98
    {0x2122, "\x99"},
    {0x0459, "\x9A"},
    {0x203A, "\x9B"},
    {0x045A, "\x9C"},
    {0x045C, "\x9D"},
    {0x045B, "\x9E"},
    {0x045F, "\x9F"},
    {0x00A0, "\xA0"},
    {0x040E, "\xA1"},
    {0x045E, "\xA2"},
    {0x0408, "\xA3"},
    {0x00A4, "\xA4"},
    {0x0490, "\xA5"},
    {0x00A6, "\xA6"},
    {0x00A7, "\xA7"},
    {0x0401, "\xA8"},
    {0x00A9, "\xA9"},
    {0x0404, "\xAA"},
    {0x00AB, "\xAB"},
    {0x00AC, "\xAC"},
    {0x00AD, "\xAD"},
    {0x00AE, "\xAE"},
    {0x0407, "\xAF"},
    {0x00B0, "\xB0"},
    {0x00B1, "\xB1"},
    {0x0406, "\xB2"},
    {0x0456, "\xB3"},
    {0x0491, "\xB4"},
    {0x00B5, "\xB5"},
    {0x00B6, "\xB6"},
    {0x00B7, "\xB7"},
    {0x0451, "\xB8"},
    {0x2116, "\xB9"},
    {0x0454, "\xBA"},
    {0x00BB, "\xBB"},
    {0x0458, "\xBC"},
    {0x0405, "\xBD"},
    {0x0455, "\xBE"},
    {0x0457, "\xBF"},
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
};

inline const auto to_cp866_map = ToCharsetMap{
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
    {0x2591, "\xB0"},
    {0x2592, "\xB1"},
    {0x2593, "\xB2"},
    {0x2502, "\xB3"},
    {0x2524, "\xB4"},
    {0x2561, "\xB5"},
    {0x2562, "\xB6"},
    {0x2556, "\xB7"},
    {0x2555, "\xB8"},
    {0x2563, "\xB9"},
    {0x2551, "\xBA"},
    {0x2557, "\xBB"},
    {0x255D, "\xBC"},
    {0x255C, "\xBD"},
    {0x255B, "\xBE"},
    {0x2510, "\xBF"},
    {0x2514, "\xC0"},
    {0x2534, "\xC1"},
    {0x252C, "\xC2"},
    {0x251C, "\xC3"},
    {0x2500, "\xC4"},
    {0x253C, "\xC5"},
    {0x255E, "\xC6"},
    {0x255F, "\xC7"},
    {0x255A, "\xC8"},
    {0x2554, "\xC9"},
    {0x2569, "\xCA"},
    {0x2566, "\xCB"},
    {0x2560, "\xCC"},
    {0x2550, "\xCD"},
    {0x256C, "\xCE"},
    {0x2567, "\xCF"},
    {0x2568, "\xD0"},
    {0x2564, "\xD1"},
    {0x2565, "\xD2"},
    {0x2559, "\xD3"},
    {0x2558, "\xD4"},
    {0x2552, "\xD5"},
    {0x2553, "\xD6"},
    {0x256B, "\xD7"},
    {0x256A, "\xD8"},
    {0x2518, "\xD9"},
    {0x250C, "\xDA"},
    {0x2588, "\xDB"},
    {0x2584, "\xDC"},
    {0x258C, "\xDD"},
    {0x2590, "\xDE"},
    {0x2580, "\xDF"},
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
    {0x0401, "\xF0"},
    {0x0451, "\xF1"},
    {0x0404, "\xF2"},
    {0x0454, "\xF3"},
    {0x0407, "\xF4"},
    {0x0457, "\xF5"},
    {0x040E, "\xF6"},
    {0x045E, "\xF7"},
    {0x00B0, "\xF8"},
    {0x2219, "\xF9"},
    {0x00B7, "\xFA"},
    {0x221A, "\xFB"},
    {0x2116, "\xFC"},
    {0x00A4, "\xFD"},
    {0x25A0, "\xFE"},
    {0x00A0, "\xFF"},
};

inline constexpr auto from_cp1251_map = FromCharsetMap{
    "\xD0\x82",     "\xD0\x83",     "\xE2\x80\x9A", "\xD1\x93",     "\xE2\x80\x9E", "\xE2\x80\xA6", "\xE2\x80\xA0",
    "\xE2\x80\xA1", "\xE2\x82\xAC", "\xE2\x80\xB0", "\xD0\x89",     "\xE2\x80\xB9", "\xD0\x8A",     "\xD0\x8C",
    "\xD0\x8B",     "\xD0\x8F",     "\xD1\x92",     "\xE2\x80\x98", "\xE2\x80\x99", "\xE2\x80\x9C", "\xE2\x80\x9D",
    "\xE2\x80\xA2", "\xE2\x80\x93", "\xE2\x80\x94", UTF_REPL_CHAR,  "\xE2\x84\xA2", "\xD1\x99",     "\xE2\x80\xBA",
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

inline constexpr auto from_cp866_map = FromCharsetMap{
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

inline auto to_charset(std::string_view str, const ToCharsetMap& to_charset_map, char repl_char) -> std::string
{
    auto res = std::string{};
    res.reserve(str.size());

    for (u32 cp : una::ranges::utf8_view(str))
    {
        if (cp < LEAST_NONASCII_SYMBOL)
        {
            res.push_back(static_cast<char>(cp));
        }
        else if (auto it = to_charset_map.find(cp); it != to_charset_map.end())
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

inline auto from_charset(std::string_view str, const FromCharsetMap& from_charset_map) -> std::string
{
    auto res = std::string{};
    res.reserve(str.size() * 3);

    for (char c : str)
    {
        const auto byte = static_cast<u8>(c);
        if (byte < LEAST_NONASCII_SYMBOL)
        {
            res.push_back(c);
        }
        else
        {
            res.append(from_charset_map[byte - LEAST_NONASCII_SYMBOL]);
        }
    }

    return res;
}

}  // namespace detail

inline auto to_translit(std::string_view str, char repl_char = '?') -> std::string
{
    return detail::to_charset(str, detail::to_translit_map, repl_char);
}

inline auto to_cp1251(std::string_view str, char repl_char = '?') -> std::string
{
    return detail::to_charset(str, detail::to_cp1251_map, repl_char);
}

inline auto to_cp866(std::string_view str, char repl_char = '?') -> std::string
{
    return detail::to_charset(str, detail::to_cp866_map, repl_char);
}

inline auto from_cp1251(std::string_view str) -> std::string
{
    return detail::from_charset(str, detail::from_cp1251_map);
}

inline auto from_cp866(std::string_view str) -> std::string
{
    return detail::from_charset(str, detail::from_cp866_map);
}

}  // namespace vsl

#endif  // VSL_ENCODING_H
