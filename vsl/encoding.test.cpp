#include "encoding.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <utility>

namespace test
{

static constexpr auto ASCII_SYMBOLS =
    R"( !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";

TEST(EncodingTest, IsAscii)
{
    EXPECT_TRUE(vsl::is_ascii(""));
    EXPECT_TRUE(vsl::is_ascii(" \n "));
    EXPECT_TRUE(vsl::is_ascii(ASCII_SYMBOLS));
    EXPECT_FALSE(vsl::is_ascii("тест"));
    EXPECT_FALSE(vsl::is_ascii("Straße"));

    EXPECT_TRUE(vsl::is_ascii("\x01"));
    EXPECT_TRUE(vsl::is_ascii("\x7F"));
    EXPECT_FALSE(vsl::is_ascii("\x80"));
    EXPECT_FALSE(vsl::is_ascii("\xFF"));
}

template<typename... Args>
auto to(Args&&... args)
{
    auto res1 = std::string{};
    res1.reserve(100);
    vsl::to_encoding(res1, std::forward<Args>(args)...);

    const auto res2 = vsl::to_encoding(std::forward<Args>(args)...);
    EXPECT_EQ(res1, res2);

    return res1;
}

template<typename... Args>
auto from(Args&&... args)
{
    auto res1 = std::string{};
    res1.reserve(100);
    vsl::from_encoding(res1, std::forward<Args>(args)...);

    const auto res2 = vsl::from_encoding(std::forward<Args>(args)...);
    EXPECT_EQ(res1, res2);

    return res1;
}

TEST(EncodingTest, ToCp1251)
{
    const auto& encoding = vsl::encodings::cp1251;

    EXPECT_EQ(to("", encoding), "");
    EXPECT_EQ(to("  \t\r\n  ", encoding), "  \t\r\n  ");

    EXPECT_EQ(to(ASCII_SYMBOLS, encoding), ASCII_SYMBOLS);
    EXPECT_EQ(to("Тест + ascii", encoding), "\xD2\xE5\xF1\xF2 + ascii");
    EXPECT_EQ(to("Ёё", encoding), "\xA8\xB8");

    EXPECT_EQ(to("Тест 🚧", encoding), "\xD2\xE5\xF1\xF2 ?");
    EXPECT_EQ(to("Тест 🚧", encoding, 'x'), "\xD2\xE5\xF1\xF2 x");
}

TEST(EncodingTest, ToCp866)
{
    const auto& encoding = vsl::encodings::cp866;

    EXPECT_EQ(to("", encoding), "");
    EXPECT_EQ(to("  \t\r\n  ", encoding), "  \t\r\n  ");

    EXPECT_EQ(to(ASCII_SYMBOLS, encoding), ASCII_SYMBOLS);
    EXPECT_EQ(to("Тест + ascii", encoding), "\x92\xA5\xE1\xE2 + ascii");
    EXPECT_EQ(to("Ёё", encoding), "\xF0\xF1");

    EXPECT_EQ(to("Тест 🚧", encoding), "\x92\xA5\xE1\xE2 ?");
    EXPECT_EQ(to("Тест 🚧", encoding, 'x'), "\x92\xA5\xE1\xE2 x");
}

TEST(EncodingTest, ToTranslit)
{
    const auto& encoding = vsl::encodings::translit_ru;

    EXPECT_EQ(to("", encoding), "");
    EXPECT_EQ(to("  \t\r\n  ", encoding), "  \t\r\n  ");

    EXPECT_EQ(to(ASCII_SYMBOLS, encoding), ASCII_SYMBOLS);
    EXPECT_EQ(to("Тест + ascii", encoding), "Test + ascii");
    EXPECT_EQ(to("Ёё", encoding), "Yoyo");

    EXPECT_EQ(to("Тест 🚧", encoding), "Test ?");
    EXPECT_EQ(to("Тест 🚧", encoding, 'x'), "Test x");

    EXPECT_EQ(to("Щука, Эхо, Съезд, Юра, Ягода", encoding), "Shhuka, E`xo, S``ezd, Yura, Yagoda");

    EXPECT_EQ(to("Привет\xC2мир", encoding), "Privet?mir");
    EXPECT_EQ(to("\x7F\x80\xD0\xA9", encoding), "\x7F?Shh");
}

TEST(EncodingTest, FromCp1251)
{
    const auto& encoding = vsl::encodings::cp1251;

    EXPECT_EQ(from("", encoding), "");
    EXPECT_EQ(from("  \t\r\n  ", encoding), "  \t\r\n  ");

    EXPECT_EQ(from(ASCII_SYMBOLS, encoding), ASCII_SYMBOLS);
    EXPECT_EQ(from("\xD2\xE5\xF1\xF2 + ascii", encoding), "Тест + ascii");

    EXPECT_EQ(from("\x01", encoding), "\x01");
    EXPECT_EQ(from("\x7F", encoding), "\x7F");
    EXPECT_EQ(from("\x80", encoding), "\xD0\x82");
    EXPECT_EQ(from("\x98", encoding), "\xEF\xBF\xBD");
    EXPECT_EQ(from("\xA8", encoding), "Ё");
    EXPECT_EQ(from("\xB8", encoding), "ё");
    EXPECT_EQ(from("\xFF", encoding), "я");
}

TEST(EncodingTest, FromCp866)
{
    const auto& encoding = vsl::encodings::cp866;

    EXPECT_EQ(from("", encoding), "");
    EXPECT_EQ(from("  \t\r\n  ", encoding), "  \t\r\n  ");

    EXPECT_EQ(from(ASCII_SYMBOLS, encoding), ASCII_SYMBOLS);
    EXPECT_EQ(from("\x92\xA5\xE1\xE2 + ascii", encoding), "Тест + ascii");

    EXPECT_EQ(from("\x01", encoding), "\x01");
    EXPECT_EQ(from("\x7F", encoding), "\x7F");
    EXPECT_EQ(from("\x80", encoding), "А");
    EXPECT_EQ(from("\xF0", encoding), "Ё");
    EXPECT_EQ(from("\xF1", encoding), "ё");
    EXPECT_EQ(from("\xFF", encoding), "\xC2\xA0");
}

TEST(EncodingTest, ToEncodingAndBack)
{
    auto test_to_encoding_and_back = [](const auto& encoding)
    {
        EXPECT_EQ(vsl::from_encoding(vsl::to_encoding("", encoding), encoding), "");
        EXPECT_EQ(vsl::from_encoding(vsl::to_encoding(ASCII_SYMBOLS, encoding), encoding), ASCII_SYMBOLS);
        EXPECT_EQ(vsl::from_encoding(vsl::to_encoding("Тест", encoding), encoding), "Тест");
        EXPECT_NE(vsl::from_encoding(vsl::to_encoding("Тест ⚠️", encoding), encoding), "Тест ⚠️");
    };

    test_to_encoding_and_back(vsl::encodings::cp1251);
    test_to_encoding_and_back(vsl::encodings::cp866);
}

}  // namespace test
