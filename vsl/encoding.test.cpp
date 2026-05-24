#include "encoding.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace vsl::test
{

static constexpr auto ASCII_SYMBOLS =
    R"( !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";

TEST(EncodingTest, IsValidUtf8)
{
    EXPECT_TRUE(vsl::is_valid_utf8(""));
    EXPECT_TRUE(vsl::is_valid_utf8(" \n "));
    EXPECT_TRUE(vsl::is_valid_utf8(ASCII_SYMBOLS));
    EXPECT_TRUE(vsl::is_valid_utf8("тест"));
    EXPECT_TRUE(vsl::is_valid_utf8("Straße"));
    EXPECT_FALSE(vsl::is_valid_utf8("\xC2"));
}

TEST(EncodingTest, IsValidUtf16)
{
    EXPECT_TRUE(vsl::is_valid_utf16(u""));
    EXPECT_TRUE(vsl::is_valid_utf16(u" \n "));
    EXPECT_TRUE(vsl::is_valid_utf16(u"test"));
    EXPECT_TRUE(vsl::is_valid_utf16(u"тест"));
    EXPECT_TRUE(vsl::is_valid_utf16(u"Straße"));
    EXPECT_FALSE(vsl::is_valid_utf16(u"\xD800"));
}

TEST(EncodingTest, IsAscii)
{
    EXPECT_TRUE(vsl::is_ascii(""));
    EXPECT_TRUE(vsl::is_ascii(" \n "));
    EXPECT_TRUE(vsl::is_ascii(ASCII_SYMBOLS));
    EXPECT_FALSE(vsl::is_ascii("тест"));
    EXPECT_FALSE(vsl::is_ascii("Straße"));

    EXPECT_TRUE(vsl::is_ascii("\x00"));
    EXPECT_TRUE(vsl::is_ascii("\x79"));
    EXPECT_FALSE(vsl::is_ascii("\x80"));
    EXPECT_FALSE(vsl::is_ascii("\xFF"));
}

TEST(EncodingTest, Utf8To16)
{
    EXPECT_EQ(vsl::utf8to16(""), u"");
    EXPECT_EQ(vsl::utf8to16("test"), u"test");
    EXPECT_EQ(vsl::utf8to16("тест"), u"тест");
    EXPECT_EQ(vsl::utf8to16("test"), u"\x74\x65\x73\x74");
    EXPECT_EQ(vsl::utf8to16("тест"), u"\x442\x435\x441\x442");
    EXPECT_EQ(vsl::utf8to16("te\xC2st"), u"te�st");
}

TEST(EncodingTest, Utf16To8)
{
    EXPECT_EQ(vsl::utf16to8(u""), "");
    EXPECT_EQ(vsl::utf16to8(u"test"), "test");
    EXPECT_EQ(vsl::utf16to8(u"тест"), "тест");
    EXPECT_EQ(vsl::utf16to8(u"\x74\x65\x73\x74"), "test");
    EXPECT_EQ(vsl::utf16to8(u"\x442\x435\x441\x442"), "тест");
    EXPECT_EQ(vsl::utf16to8(u"te\xD800st"), "te�st");
}

TEST(EncodingTest, ToTranslit)
{
    EXPECT_EQ(vsl::to_translit(""), "");
    EXPECT_EQ(vsl::to_translit("  \t\r\n  "), "  \t\r\n  ");

    EXPECT_EQ(vsl::to_translit(ASCII_SYMBOLS), ASCII_SYMBOLS);
    EXPECT_EQ(vsl::to_translit("Тест + ascii"), "Test + ascii");
    EXPECT_EQ(vsl::to_translit("Ёё"), "Yoyo");

    EXPECT_EQ(vsl::to_translit("Тест 🚧"), "Test ?");
    EXPECT_EQ(vsl::to_translit("Тест 🚧", 'x'), "Test x");

    EXPECT_EQ(vsl::to_translit("Щука, Эхо, Съезд, Юра, Ягода"), "Shhuka, E`xo, S``ezd, Yura, Yagoda");

    EXPECT_EQ(vsl::to_translit("Привет\xC2мир"), "Privet?mir");
    EXPECT_EQ(vsl::to_translit("\x79\x80\xD0\xA9"), "\x79?Shh");
}

TEST(EncodingTest, ToCp1251)
{
    EXPECT_EQ(vsl::to_cp1251(""), "");
    EXPECT_EQ(vsl::to_cp1251("  \t\r\n  "), "  \t\r\n  ");

    EXPECT_EQ(vsl::to_cp1251(ASCII_SYMBOLS), ASCII_SYMBOLS);
    EXPECT_EQ(vsl::to_cp1251("Тест + ascii"), "\xD2\xE5\xF1\xF2 + ascii");
    EXPECT_EQ(vsl::to_cp1251("Ёё"), "\xA8\xB8");

    EXPECT_EQ(vsl::to_cp1251("Тест 🚧"), "\xD2\xE5\xF1\xF2 ?");
    EXPECT_EQ(vsl::to_cp1251("Тест 🚧", 'x'), "\xD2\xE5\xF1\xF2 x");
}

TEST(EncodingTest, ToCp866)
{
    EXPECT_EQ(vsl::to_cp866(""), "");
    EXPECT_EQ(vsl::to_cp866("  \t\r\n  "), "  \t\r\n  ");

    EXPECT_EQ(vsl::to_cp866(ASCII_SYMBOLS), ASCII_SYMBOLS);
    EXPECT_EQ(vsl::to_cp866("Тест + ascii"), "\x92\xA5\xE1\xE2 + ascii");
    EXPECT_EQ(vsl::to_cp866("Ёё"), "\xF0\xF1");

    EXPECT_EQ(vsl::to_cp866("Тест 🚧"), "\x92\xA5\xE1\xE2 ?");
    EXPECT_EQ(vsl::to_cp866("Тест 🚧", 'x'), "\x92\xA5\xE1\xE2 x");
}

TEST(EncodingTest, FromCp1251)
{
    EXPECT_EQ(vsl::from_cp1251(""), "");
    EXPECT_EQ(vsl::from_cp1251("  \t\r\n  "), "  \t\r\n  ");

    EXPECT_EQ(vsl::from_cp1251(ASCII_SYMBOLS), ASCII_SYMBOLS);
    EXPECT_EQ(vsl::from_cp1251("\xD2\xE5\xF1\xF2 + ascii"), "Тест + ascii");

    EXPECT_EQ(vsl::from_cp1251("\x00"), "\x00");
    EXPECT_EQ(vsl::from_cp1251("\x79"), "\x79");
    EXPECT_EQ(vsl::from_cp1251("\x80"), "\xD0\x82");
    EXPECT_EQ(vsl::from_cp1251("\x98"), "\xEF\xBF\xBD");
    EXPECT_EQ(vsl::from_cp1251("\xA8"), "Ё");
    EXPECT_EQ(vsl::from_cp1251("\xB8"), "ё");
    EXPECT_EQ(vsl::from_cp1251("\xFF"), "я");
}

TEST(EncodingTest, FromCp866)
{
    EXPECT_EQ(vsl::from_cp866(""), "");
    EXPECT_EQ(vsl::from_cp866("  \t\r\n  "), "  \t\r\n  ");

    EXPECT_EQ(vsl::from_cp866(ASCII_SYMBOLS), ASCII_SYMBOLS);
    EXPECT_EQ(vsl::from_cp866("\x92\xA5\xE1\xE2 + ascii"), "Тест + ascii");

    EXPECT_EQ(vsl::from_cp866("\x00"), "\x00");
    EXPECT_EQ(vsl::from_cp866("\x79"), "\x79");
    EXPECT_EQ(vsl::from_cp866("\x80"), "А");
    EXPECT_EQ(vsl::from_cp866("\xF0"), "Ё");
    EXPECT_EQ(vsl::from_cp866("\xF1"), "ё");
    EXPECT_EQ(vsl::from_cp866("\xFF"), "\xC2\xA0");
}

TEST(EncodingTest, ToCharsetAndBack)
{
    EXPECT_EQ(vsl::from_cp1251(vsl::to_cp1251("")), "");
    EXPECT_EQ(vsl::from_cp1251(vsl::to_cp1251(ASCII_SYMBOLS)), ASCII_SYMBOLS);
    EXPECT_EQ(vsl::from_cp1251(vsl::to_cp1251("Тест")), "Тест");
    EXPECT_NE(vsl::from_cp1251(vsl::to_cp1251("Тест ⚠️")), "Тест ⚠️");

    EXPECT_EQ(vsl::from_cp866(vsl::to_cp866("")), "");
    EXPECT_EQ(vsl::from_cp866(vsl::to_cp866(ASCII_SYMBOLS)), ASCII_SYMBOLS);
    EXPECT_EQ(vsl::from_cp866(vsl::to_cp866("Тест")), "Тест");
    EXPECT_NE(vsl::from_cp866(vsl::to_cp866("Тест ⚠️")), "Тест ⚠️");
}

}  // namespace vsl::test
