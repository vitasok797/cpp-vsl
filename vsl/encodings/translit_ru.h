#ifndef VSL_ENCODINGS_TRANSLIT_RU_H
#define VSL_ENCODINGS_TRANSLIT_RU_H

#include <vsl/encodings/detail.h>

namespace vsl::encodings
{

namespace detail::translit_ru
{

// Apostrophe/grave symbol
inline constexpr unsigned char AP = '`';

// GOST 7.79-2000, System B
struct Encoding
{
    // --- To encoding ----------------------------------------------------------------------------

    static inline constexpr ToEncodingMap<uint16_t, 3, 66> to_encoding_map{
        {
         {0x0401, {'Y', 'o', 0}}, {0x0410, {'A', 0, 0}},   {0x0411, {'B', 0, 0}},     {0x0412, {'V', 0, 0}},
         {0x0413, {'G', 0, 0}},   {0x0414, {'D', 0, 0}},   {0x0415, {'E', 0, 0}},     {0x0416, {'Z', 'h', 0}},
         {0x0417, {'Z', 0, 0}},   {0x0418, {'I', 0, 0}},   {0x0419, {'J', 0, 0}},     {0x041A, {'K', 0, 0}},
         {0x041B, {'L', 0, 0}},   {0x041C, {'M', 0, 0}},   {0x041D, {'N', 0, 0}},     {0x041E, {'O', 0, 0}},
         {0x041F, {'P', 0, 0}},   {0x0420, {'R', 0, 0}},   {0x0421, {'S', 0, 0}},     {0x0422, {'T', 0, 0}},
         {0x0423, {'U', 0, 0}},   {0x0424, {'F', 0, 0}},   {0x0425, {'X', 0, 0}},     {0x0426, {'C', 'z', 0}},
         {0x0427, {'C', 'h', 0}}, {0x0428, {'S', 'h', 0}}, {0x0429, {'S', 'h', 'h'}}, {0x042A, {AP, AP, 0}},
         {0x042B, {'Y', AP, 0}},  {0x042C, {AP, 0, 0}},    {0x042D, {'E', AP, 0}},    {0x042E, {'Y', 'u', 0}},
         {0x042F, {'Y', 'a', 0}}, {0x0430, {'a', 0, 0}},   {0x0431, {'b', 0, 0}},     {0x0432, {'v', 0, 0}},
         {0x0433, {'g', 0, 0}},   {0x0434, {'d', 0, 0}},   {0x0435, {'e', 0, 0}},     {0x0436, {'z', 'h', 0}},
         {0x0437, {'z', 0, 0}},   {0x0438, {'i', 0, 0}},   {0x0439, {'j', 0, 0}},     {0x043A, {'k', 0, 0}},
         {0x043B, {'l', 0, 0}},   {0x043C, {'m', 0, 0}},   {0x043D, {'n', 0, 0}},     {0x043E, {'o', 0, 0}},
         {0x043F, {'p', 0, 0}},   {0x0440, {'r', 0, 0}},   {0x0441, {'s', 0, 0}},     {0x0442, {'t', 0, 0}},
         {0x0443, {'u', 0, 0}},   {0x0444, {'f', 0, 0}},   {0x0445, {'x', 0, 0}},     {0x0446, {'c', 'z', 0}},
         {0x0447, {'c', 'h', 0}}, {0x0448, {'s', 'h', 0}}, {0x0449, {'s', 'h', 'h'}}, {0x044A, {AP, AP, 0}},
         {0x044B, {'y', AP, 0}},  {0x044C, {AP, 0, 0}},    {0x044D, {'e', AP, 0}},    {0x044E, {'y', 'u', 0}},
         {0x044F, {'y', 'a', 0}}, {0x0451, {'y', 'o', 0}},
         }
    };

    auto to_encoding_reserve(std::string& out, std::string_view str) const -> void
    {
        out.reserve(str.size() + str.size() / 2);  // +50%
    }

    // --------------------------------------------------------------------------------------------
};

}  // namespace detail::translit_ru

inline constexpr auto translit_ru = detail::translit_ru::Encoding{};

}  // namespace vsl::encodings

#endif  // VSL_ENCODINGS_TRANSLIT_RU_H
