#ifndef VSL_ENCODINGS_TRANSLIT_RU_H
#define VSL_ENCODINGS_TRANSLIT_RU_H

#include <vsl/encodings/detail.h>

#include <array>
#include <string>
#include <string_view>

namespace vsl::encodings
{

namespace detail
{

// GOST 7.79-2000, System B
struct translit_ru_t
{
    static inline constexpr std::array<ToEncodingMapItem, 66> to_encoding_map = std::to_array<ToEncodingMapItem>({
        {0x0401,  "Yo"}, // Ё
        {0x0410,   "A"}, // А
        {0x0411,   "B"}, // Б
        {0x0412,   "V"}, // В
        {0x0413,   "G"}, // Г
        {0x0414,   "D"}, // Д
        {0x0415,   "E"}, // Е
        {0x0416,  "Zh"}, // Ж
        {0x0417,   "Z"}, // З
        {0x0418,   "I"}, // И
        {0x0419,   "J"}, // Й
        {0x041A,   "K"}, // К
        {0x041B,   "L"}, // Л
        {0x041C,   "M"}, // М
        {0x041D,   "N"}, // Н
        {0x041E,   "O"}, // О
        {0x041F,   "P"}, // П
        {0x0420,   "R"}, // Р
        {0x0421,   "S"}, // С
        {0x0422,   "T"}, // Т
        {0x0423,   "U"}, // У
        {0x0424,   "F"}, // Ф
        {0x0425,   "X"}, // Х
        {0x0426,  "Cz"}, // Ц
        {0x0427,  "Ch"}, // Ч
        {0x0428,  "Sh"}, // Ш
        {0x0429, "Shh"}, // Щ
        {0x042A,  "``"}, // Ъ
        {0x042B,  "Y`"}, // Ы
        {0x042C,   "`"}, // Ь
        {0x042D,  "E`"}, // Э
        {0x042E,  "Yu"}, // Ю
        {0x042F,  "Ya"}, // Я
        {0x0430,   "a"}, // а
        {0x0431,   "b"}, // б
        {0x0432,   "v"}, // в
        {0x0433,   "g"}, // г
        {0x0434,   "d"}, // д
        {0x0435,   "e"}, // е
        {0x0436,  "zh"}, // ж
        {0x0437,   "z"}, // з
        {0x0438,   "i"}, // и
        {0x0439,   "j"}, // й
        {0x043A,   "k"}, // к
        {0x043B,   "l"}, // л
        {0x043C,   "m"}, // м
        {0x043D,   "n"}, // н
        {0x043E,   "o"}, // о
        {0x043F,   "p"}, // п
        {0x0440,   "r"}, // р
        {0x0441,   "s"}, // с
        {0x0442,   "t"}, // т
        {0x0443,   "u"}, // у
        {0x0444,   "f"}, // ф
        {0x0445,   "x"}, // х
        {0x0446,  "cz"}, // ц
        {0x0447,  "ch"}, // ч
        {0x0448,  "sh"}, // ш
        {0x0449, "shh"}, // щ
        {0x044A,  "``"}, // ъ
        {0x044B,  "y`"}, // ы
        {0x044C,   "`"}, // ь
        {0x044D,  "e`"}, // э
        {0x044E,  "yu"}, // ю
        {0x044F,  "ya"}, // я
        {0x0451,  "yo"}, // ё
    });

    auto to_encoding_reserve(std::string& out, std::string_view str) const -> void
    {
        out.reserve(str.size() + str.size() / 2);  // +50%
    }
};

}  // namespace detail

inline constexpr auto translit_ru = detail::translit_ru_t{};

}  // namespace vsl::encodings

#endif  // VSL_ENCODINGS_TRANSLIT_RU_H
