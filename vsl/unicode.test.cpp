#include "unicode.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <string_view>

namespace test
{

static constexpr auto ASCII_SYMBOLS =
    R"( !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";

TEST(UnicodeTest, IsValidUtf16)
{
    EXPECT_TRUE(vsl::is_valid_utf16(u""));
    EXPECT_TRUE(vsl::is_valid_utf16(u" \n "));
    EXPECT_TRUE(vsl::is_valid_utf16(u"test"));
    EXPECT_TRUE(vsl::is_valid_utf16(u"тест"));
    EXPECT_TRUE(vsl::is_valid_utf16(u"Straße"));
    EXPECT_FALSE(vsl::is_valid_utf16(u"\xD800"));

    // Input string types
    EXPECT_TRUE(vsl::is_valid_utf16(u"тест"));
    EXPECT_TRUE(vsl::is_valid_utf16(std::u16string{u"тест"}));
    EXPECT_TRUE(vsl::is_valid_utf16(std::u16string_view{u"тест"}));
}

TEST(UnicodeTest, IsValidUtf8)
{
    EXPECT_TRUE(vsl::is_valid_utf8(""));
    EXPECT_TRUE(vsl::is_valid_utf8(" \n "));
    EXPECT_TRUE(vsl::is_valid_utf8(ASCII_SYMBOLS));
    EXPECT_TRUE(vsl::is_valid_utf8("тест"));
    EXPECT_TRUE(vsl::is_valid_utf8("Straße"));
    EXPECT_FALSE(vsl::is_valid_utf8("\xC2"));

    // Input string types
    EXPECT_TRUE(vsl::is_valid_utf8("тест"));
    EXPECT_TRUE(vsl::is_valid_utf8(std::string{"тест"}));
    EXPECT_TRUE(vsl::is_valid_utf8(std::string_view{"тест"}));
}

TEST(UnicodeTest, Normalization)
{
    EXPECT_EQ(vsl::to_nfc_utf8("Ŵ W\u0302"), "Ŵ Ŵ");
    EXPECT_EQ(vsl::to_nfd_utf8("Ŵ W\u0302"), "W\u0302 W\u0302");

    EXPECT_EQ(vsl::to_nfkc_utf8("ﬃ 2\u2075 Ŵ W\u0302"), "ffi 25 Ŵ Ŵ");
    EXPECT_EQ(vsl::to_nfkd_utf8("ﬃ 2\u2075 Ŵ W\u0302"), "ffi 25 W\u0302 W\u0302");

    EXPECT_TRUE(vsl::is_nfc_utf8("Ŵ"));
    EXPECT_TRUE(vsl::is_nfc_utf8("ﬃ"));
    EXPECT_FALSE(vsl::is_nfc_utf8("W\u0302"));

    EXPECT_TRUE(vsl::is_nfd_utf8("W\u0302"));
    EXPECT_TRUE(vsl::is_nfd_utf8("ﬃ"));
    EXPECT_FALSE(vsl::is_nfd_utf8("Ŵ"));

    EXPECT_TRUE(vsl::is_nfkc_utf8("Ŵ"));
    EXPECT_FALSE(vsl::is_nfkc_utf8("W\u0302"));
    EXPECT_FALSE(vsl::is_nfkc_utf8("ﬃ"));

    EXPECT_TRUE(vsl::is_nfkd_utf8("W\u0302"));
    EXPECT_FALSE(vsl::is_nfkd_utf8("Ŵ"));
    EXPECT_FALSE(vsl::is_nfkd_utf8("ﬃ"));

    // Input string types
    EXPECT_EQ(vsl::to_nfc_utf8("W\u0302"), "Ŵ");
    EXPECT_EQ(vsl::to_nfc_utf8(std::string{"W\u0302"}), "Ŵ");
    EXPECT_EQ(vsl::to_nfc_utf8(std::string_view{"W\u0302"}), "Ŵ");
    EXPECT_TRUE(vsl::is_nfc_utf8("Ŵ"));
    EXPECT_TRUE(vsl::is_nfc_utf8(std::string{"Ŵ"}));
    EXPECT_TRUE(vsl::is_nfc_utf8(std::string_view{"Ŵ"}));
}

TEST(UnicodeTest, Utf16To8)
{
    EXPECT_EQ(vsl::utf16to8(u""), "");
    EXPECT_EQ(vsl::utf16to8(u"test"), "test");
    EXPECT_EQ(vsl::utf16to8(u"тест"), "тест");
    EXPECT_EQ(vsl::utf16to8(u"\x74\x65\x73\x74"), "test");
    EXPECT_EQ(vsl::utf16to8(u"\x442\x435\x441\x442"), "тест");
    EXPECT_EQ(vsl::utf16to8(u"te\xD800st"), "te�st");
}

TEST(UnicodeTest, Utf8To16)
{
    EXPECT_EQ(vsl::utf8to16(""), u"");
    EXPECT_EQ(vsl::utf8to16("test"), u"test");
    EXPECT_EQ(vsl::utf8to16("тест"), u"тест");
    EXPECT_EQ(vsl::utf8to16("test"), u"\x74\x65\x73\x74");
    EXPECT_EQ(vsl::utf8to16("тест"), u"\x442\x435\x441\x442");
    EXPECT_EQ(vsl::utf8to16("te\xC2st"), u"te�st");
}

}  // namespace test
