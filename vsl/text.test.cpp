#include "text.h"

#include <vsl/concepts.h>
#include <vsl/types.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace test
{

TEST(TextTest, Case)
{
    EXPECT_EQ(vsl::to_upper("Straße"), "STRASSE");
    EXPECT_EQ(vsl::to_lower("LIGATURE ﬃ"), "ligature ﬃ");
    EXPECT_EQ(vsl::to_lower("ДВА"), "два");
    EXPECT_EQ(vsl::to_casefold("LIGATURE ﬃ"), "ligature ffi");
    EXPECT_EQ(vsl::to_titlecase("teMPuS eDAX reRuM"), "Tempus Edax Rerum");
}

TEST(TextTest, CaseAscii)
{
    EXPECT_EQ(vsl::to_upper_ascii(""), "");
    EXPECT_EQ(vsl::to_lower_ascii(""), "");

    EXPECT_EQ(vsl::to_upper_ascii(" \n 123 "), " \n 123 ");
    EXPECT_EQ(vsl::to_lower_ascii(" \n 123 "), " \n 123 ");

    EXPECT_EQ(vsl::to_upper_ascii("az AZ az"), "AZ AZ AZ");
    EXPECT_EQ(vsl::to_lower_ascii("az AZ az"), "az az az");

    EXPECT_EQ(vsl::to_upper_ascii("ЮНИкод"), "ЮНИкод");
    EXPECT_EQ(vsl::to_lower_ascii("ЮНИкод"), "ЮНИкод");
}

TEST(TextTest, IsEqual)
{
    EXPECT_TRUE(vsl::is_equal("юникод", "юникод"));
    EXPECT_FALSE(vsl::is_equal("ЮНИКОД", "юникод"));
    EXPECT_TRUE(vsl::is_equal_icase("ЮНИКОД", "юникод"));

    EXPECT_TRUE(vsl::is_equal("ﬃ", "ﬃ"));
    EXPECT_FALSE(vsl::is_equal("ﬃ", "ffi"));
    EXPECT_TRUE(vsl::is_equal_icase("ﬃ", "ffi"));

    EXPECT_TRUE(vsl::is_equal_icase("Straße", "STRASSE"));
    EXPECT_FALSE(vsl::is_equal_icase("тест", "текст"));
}

TEST(TextTest, IsEqualAsciiIcase)
{
    EXPECT_FALSE(vsl::is_equal_ascii_icase("test", "test1"));  // Different size

    EXPECT_TRUE(vsl::is_equal_ascii_icase("", ""));
    EXPECT_TRUE(vsl::is_equal_ascii_icase(" ", " "));

    EXPECT_TRUE(vsl::is_equal_ascii_icase("test", "test"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase("test", "text"));

    EXPECT_TRUE(vsl::is_equal_ascii_icase("Test", "test"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase("Test", "xest"));

    EXPECT_TRUE(vsl::is_equal_ascii_icase("teSt", "test"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase("teSt", "text"));

    EXPECT_TRUE(vsl::is_equal_ascii_icase("tesT", "test"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase("tesT", "tesx"));

    EXPECT_TRUE(vsl::is_equal_ascii_icase("TEST", "test"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase("TEST", "xxxx"));

    EXPECT_TRUE(vsl::is_equal_ascii_icase("123", "123"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase("123", "124"));

    EXPECT_TRUE(vsl::is_equal_ascii_icase("!@#$%", "!@#$%"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase("!@#$%", "!@#$_"));

    EXPECT_TRUE(vsl::is_equal_ascii_icase("a1z", "a1z"));
    EXPECT_TRUE(vsl::is_equal_ascii_icase("A1Z", "a1z"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase("a1z", "a2z"));

    EXPECT_TRUE(vsl::is_equal_ascii_icase("a!z", "a!z"));
    EXPECT_TRUE(vsl::is_equal_ascii_icase("A!Z", "a!z"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase("a!z", "a@z"));

    EXPECT_TRUE(vsl::is_equal_ascii_icase("a\x01z", "a\x01z"));
    EXPECT_TRUE(vsl::is_equal_ascii_icase("A\x01Z", "a\x01z"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase("a\x01z", "a\x02z"));

    // ASCII letter range boundaries
    EXPECT_TRUE(vsl::is_equal_ascii_icase("a", "A"));
    EXPECT_TRUE(vsl::is_equal_ascii_icase("z", "Z"));
    EXPECT_FALSE(vsl::is_equal_ascii_icase(std::string{'a' - 1}, std::string{'A' - 1}));  // '`' and '@'
    EXPECT_FALSE(vsl::is_equal_ascii_icase(std::string{'z' + 1}, std::string{'Z' + 1}));  // '{' and '['

    constexpr auto CHUNK_SIZE = static_cast<std::string::size_type>(vsl::IS_EQUAL_ASCII_ICASE_CHUNK_SIZE);

    auto test_multichunk = [](bool expected_res, size_t len, char fill_char2,
                              std::string::size_type mismatch_pos = std::string::npos, char mismatch_char2 = 0)
    {
        auto str1 = std::string(len, 'a');
        auto str2 = std::string(len, fill_char2);
        if (mismatch_pos != std::string::npos)
        {
            str2[mismatch_pos] = mismatch_char2;
        }
        EXPECT_EQ(vsl::is_equal_ascii_icase(str1, str2), expected_res);
    };

    // Exact match
    {
        test_multichunk(true, CHUNK_SIZE, 'a');
        test_multichunk(true, CHUNK_SIZE, 'A');
        test_multichunk(true, CHUNK_SIZE + CHUNK_SIZE / 2, 'a');
        test_multichunk(true, CHUNK_SIZE + CHUNK_SIZE / 2, 'A');
        test_multichunk(true, CHUNK_SIZE * 2, 'a');
        test_multichunk(true, CHUNK_SIZE * 2, 'A');
    }

    // Mismatch at the string beginning
    {
        const auto len = CHUNK_SIZE * 2;
        const auto pos = std::string::size_type{0};
        test_multichunk(true, len, 'a', pos, 'A');
        test_multichunk(false, len, 'a', pos, 'b');
    }

    // Mismatch at the chunk boundary
    {
        const auto len = CHUNK_SIZE * 2;
        const auto pos = CHUNK_SIZE;
        test_multichunk(true, len, 'a', pos - 1, 'A');
        test_multichunk(true, len, 'a', pos, 'A');
        test_multichunk(false, len, 'a', pos - 1, 'b');
        test_multichunk(false, len, 'a', pos, 'b');
    }

    // Mismatch at the string end
    {
        const auto len = CHUNK_SIZE * 2;
        const auto pos = len - 1;
        test_multichunk(true, len, 'a', pos, 'A');
        test_multichunk(false, len, 'a', pos, 'b');
    }

    // Mismatch at the string end (incomplete last chunk)
    {
        const auto len = CHUNK_SIZE + CHUNK_SIZE / 2;
        const auto pos = len - 1;
        test_multichunk(true, len, 'a', pos, 'A');
        test_multichunk(false, len, 'a', pos, 'b');
    }

    // 'Zebra' pattern on a large string
    {
        const auto len = CHUNK_SIZE * 3;
        auto str1 = std::string(len, ' ');
        auto str2 = std::string(len, ' ');

        for (size_t i = 0; i < len; ++i)
        {
            if (i % 2 == 0)
            {
                str1[i] = 'E';
                str2[i] = 'e';
            }
            else
            {
                str1[i] = 'f';
                str2[i] = 'F';
            }
        }

        EXPECT_TRUE(vsl::is_equal_ascii_icase(str1, str2));
    }
}

TEST(TextTest, CompareStr)
{
    // TODO: add vsl::compare_str tests
}

TEST(TextTest, Collate)
{
    EXPECT_EQ(vsl::collate_str("арбуз", "арбуз"), 0);
    EXPECT_GT(vsl::collate_str("Арбуз", "арбуз"), 0);
    EXPECT_LT(vsl::collate_str("арбуз", "Арбуз"), 0);

    EXPECT_EQ(vsl::collate_str_icase("арбуз", "Арбуз"), 0);
    EXPECT_EQ(vsl::collate_str_icase("Арбуз", "арбуз"), 0);
    EXPECT_EQ(vsl::collate_str_icase("LIGATURE ﬃ", "ligature ffi"), 0);

    auto vec = std::vector<std::string>{"браво", "астра", "АЛЬФА", "ОМЕГА"};
    std::ranges::sort(vec);
    EXPECT_THAT(vec, testing::ElementsAre("АЛЬФА", "ОМЕГА", "астра", "браво"));
    std::ranges::sort(vec, [](auto a, auto b) { return vsl::collate_str(a, b) < 0; });
    EXPECT_THAT(vec, testing::ElementsAre("астра", "АЛЬФА", "браво", "ОМЕГА"));
    std::ranges::sort(vec, [](auto a, auto b) { return vsl::collate_str_icase(a, b) < 0; });
    EXPECT_THAT(vec, testing::ElementsAre("АЛЬФА", "астра", "браво", "ОМЕГА"));
}

TEST(TextTest, FindStr)
{
    // operator bool()
    const vsl::FoundStr found = vsl::find_str("раз два три", "два");
    EXPECT_TRUE(found);

    // icase
    EXPECT_TRUE(vsl::find_str("раз два три", "два"));
    EXPECT_FALSE(vsl::find_str("раз два три", "ДВА"));
    EXPECT_TRUE(vsl::find_str_icase("раз два три", "ДВА"));

    // icase
    EXPECT_TRUE(vsl::find_str("Ligature ﬃ search", "ﬃ"));
    EXPECT_FALSE(vsl::find_str("Ligature ﬃ search", "ffi"));
    EXPECT_TRUE(vsl::find_str_icase("Ligature ﬃ search", "ffi"));

    // pos + icase
    const vsl::FoundStr found2 = vsl::find_str_icase("Ligature ﬃ search", "ffi");
    EXPECT_EQ(found2.pos(), 9);
    EXPECT_EQ(found2.end_pos(), 12);
}

static const auto NBSP = std::string{"\xC2\xA0"};
static const auto WHITESPACES = std::string{" \n\r\t\f\v"};

TEST(TextTest, Trim)
{
    EXPECT_EQ(vsl::trim_start(""), "");
    EXPECT_EQ(vsl::trim_end(""), "");
    EXPECT_EQ(vsl::trim(""), "");

    EXPECT_EQ(vsl::trim_start("abc"), "abc");
    EXPECT_EQ(vsl::trim_end("abc"), "abc");
    EXPECT_EQ(vsl::trim("abc"), "abc");

    EXPECT_EQ(vsl::trim(WHITESPACES), "");
    EXPECT_EQ(vsl::trim(NBSP), "");
    EXPECT_EQ(vsl::trim(NBSP + NBSP), "");

    EXPECT_EQ(vsl::trim_start(NBSP + WHITESPACES + NBSP + NBSP + "юникод" + NBSP + NBSP + WHITESPACES + NBSP),
              "юникод" + NBSP + NBSP + WHITESPACES + NBSP);
    EXPECT_EQ(vsl::trim_end(NBSP + WHITESPACES + NBSP + NBSP + "юникод" + NBSP + NBSP + WHITESPACES + NBSP),
              NBSP + WHITESPACES + NBSP + NBSP + "юникод");
    EXPECT_EQ(vsl::trim(NBSP + WHITESPACES + NBSP + NBSP + "юникод" + NBSP + NBSP + WHITESPACES + NBSP), "юникод");

    EXPECT_EQ(vsl::trim("  \xC2 \xA0 юникод \xC2 \xA0  "), "\xC2 \xA0 юникод \xC2 \xA0");
    EXPECT_EQ(vsl::trim("  \xC2\xBD юникод \xD0\xA0  "), "\xC2\xBD юникод \xD0\xA0");
}

TEST(TextTest, TrimAscii)
{
    EXPECT_EQ(vsl::trim_ascii_start(""), "");
    EXPECT_EQ(vsl::trim_ascii_end(""), "");
    EXPECT_EQ(vsl::trim_ascii(""), "");

    EXPECT_EQ(vsl::trim_ascii_start("abc"), "abc");
    EXPECT_EQ(vsl::trim_ascii_end("abc"), "abc");
    EXPECT_EQ(vsl::trim_ascii("abc"), "abc");

    EXPECT_EQ(vsl::trim_ascii(WHITESPACES), "");
    EXPECT_EQ(vsl::trim_ascii(NBSP), NBSP);
    EXPECT_EQ(vsl::trim_ascii(NBSP + NBSP), NBSP + NBSP);

    EXPECT_EQ(vsl::trim_ascii_start(WHITESPACES + "юникод" + WHITESPACES), "юникод" + WHITESPACES);
    EXPECT_EQ(vsl::trim_ascii_end(WHITESPACES + "юникод" + WHITESPACES), WHITESPACES + "юникод");
    EXPECT_EQ(vsl::trim_ascii(WHITESPACES + "юникод" + WHITESPACES), "юникод");
    EXPECT_EQ(vsl::trim_ascii(WHITESPACES + NBSP + "юникод" + NBSP + WHITESPACES), NBSP + "юникод" + NBSP);

    const auto symbols = "$%";
    EXPECT_EQ(vsl::trim_ascii_start("$$%%$$  юникод  $$%%$$", symbols), "  юникод  $$%%$$");
    EXPECT_EQ(vsl::trim_ascii_end("$$%%$$  юникод  $$%%$$", symbols), "$$%%$$  юникод  ");
    EXPECT_EQ(vsl::trim_ascii("$$%%$$  юникод  $$%%$$", symbols), "  юникод  ");
}

TEST(TextTest, Split)
{
    auto test_split = [](auto&& s, auto&& sep, auto&& opt, auto&& expected_res)
    {
        using namespace testing;

        {
            auto res = std::vector<std::string>{};
            res.reserve(100);
            vsl::split(res, s, sep, opt);
            EXPECT_THAT(res, ElementsAreArray(expected_res));
        }
        {
            const auto res = vsl::split(s, sep, opt);
            static_assert(vsl::same_type_as<decltype(res), std::vector<std::string>>);
            EXPECT_THAT(res, ElementsAreArray(expected_res));
        }
        {
            auto res = std::vector<std::string_view>{};
            res.reserve(100);
            vsl::split(res, s, sep, opt);
            EXPECT_THAT(res, ElementsAreArray(expected_res));
        }
        {
            const auto res = vsl::split<std::string_view>(s, sep, opt);
            static_assert(vsl::same_type_as<decltype(res), std::vector<std::string_view>>);
            EXPECT_THAT(res, ElementsAreArray(expected_res));
        }
    };

    const auto sep = ";";
    const auto no_opt = vsl::SplitOptions::NONE;
    const auto empty_res = std::vector<std::string_view>{};

    {
        const auto s = "";
        const auto expected_res = {""};  // NOTE: the result is different for re_split/split
        test_split(s, sep, no_opt, expected_res);
    }
    {
        const auto s = "";
        const auto opt = vsl::SplitOptions::SKIP_EMPTY;
        const auto expected_res = empty_res;
        test_split(s, sep, opt, expected_res);
    }
    {
        const auto s = ";";
        const auto expected_res = {"", ""};  // NOTE: the result is different for re_split/split
        test_split(s, sep, no_opt, expected_res);
    }
    {
        const auto s = ";;";
        const auto expected_res = {"", "", ""};  // NOTE: the result is different for re_split/split
        test_split(s, sep, no_opt, expected_res);
    }
    {
        const auto s = ";;";
        const auto opt = vsl::SplitOptions::SKIP_EMPTY;
        const auto expected_res = empty_res;
        test_split(s, sep, opt, expected_res);
    }
    {
        const auto s = "1";
        const auto expected_res = {"1"};
        test_split(s, sep, no_opt, expected_res);
    }
    {
        const auto s = " 1 ";
        const auto opt = vsl::SplitOptions::TRIM;
        const auto expected_res = {"1"};
        test_split(s, sep, opt, expected_res);
    }
    {
        const auto s = "  ";
        const auto opt = vsl::SplitOptions::TRIM;
        const auto expected_res = {""};
        test_split(s, sep, opt, expected_res);
    }
    {
        const auto s = "  ";
        const auto opt = vsl::SplitOptions::TRIM | vsl::SplitOptions::SKIP_EMPTY;
        const auto expected_res = empty_res;
        test_split(s, sep, opt, expected_res);
    }
    {
        const auto s = "ЭфываЮфываЯ";
        const auto utf8_sep = "фыва";
        const auto expected_res = {"Э", "Ю", "Я"};
        test_split(s, utf8_sep, no_opt, expected_res);
    }
    {
        const auto s = "1;2;3";
        const auto expected_res = {"1", "2", "3"};
        test_split(s, sep, no_opt, expected_res);
    }
    {
        const auto s = " 1; 2; 3 \n";
        const auto expected_res = {" 1", " 2", " 3 \n"};
        test_split(s, sep, no_opt, expected_res);
    }
    {
        const auto s = " 1; 2; 3 \n";
        const auto opt = vsl::SplitOptions::TRIM;
        const auto expected_res = {"1", "2", "3"};
        test_split(s, sep, opt, expected_res);
    }
    {
        const auto s = ";1;;3;";
        const auto expected_res = {"", "1", "", "3", ""};  // NOTE: the result is different for re_split/split
        test_split(s, sep, no_opt, expected_res);
    }
    {
        const auto s = ";1;;3;";
        const auto opt = vsl::SplitOptions::SKIP_EMPTY;
        const auto expected_res = {"1", "3"};
        test_split(s, sep, opt, expected_res);
    }
    {
        const auto s = " ;1; ;3; ";
        const auto opt = vsl::SplitOptions::SKIP_EMPTY;
        const auto expected_res = {" ", "1", " ", "3", " "};
        test_split(s, sep, opt, expected_res);
    }
    {
        const auto s = " ;1; ;3; ";
        const auto opt = vsl::SplitOptions::TRIM | vsl::SplitOptions::SKIP_EMPTY;
        const auto expected_res = {"1", "3"};
        test_split(s, sep, opt, expected_res);
    }
    {
        const auto s = "a";
        const auto custom_sep = "abc";
        const auto expected_res = {"a"};
        test_split(s, custom_sep, no_opt, expected_res);
    }
    {
        const auto s = "aaa";
        const auto custom_sep = "aa";
        const auto expected_res = {"", "a"};
        test_split(s, custom_sep, no_opt, expected_res);
    }

    // Empty separator

    {
        const auto s = "  1;2;3  ";
        const auto empty_sep = "";
        const auto expected_res = {"  1;2;3  "};
        test_split(s, empty_sep, no_opt, expected_res);
    }
    {
        const auto s = "  1;2;3  ";
        const auto empty_sep = "";
        const auto opt = vsl::SplitOptions::TRIM;
        const auto expected_res = {"1;2;3"};
        test_split(s, empty_sep, opt, expected_res);
    }
    {
        const auto s = "  ";
        const auto empty_sep = "";
        const auto opt = vsl::SplitOptions::TRIM | vsl::SplitOptions::SKIP_EMPTY;
        const auto expected_res = empty_res;
        test_split(s, empty_sep, opt, expected_res);
    }
    {
        const auto s = "";
        const auto empty_sep = "";
        const auto expected_res = {""};
        test_split(s, empty_sep, no_opt, expected_res);
    }
}

TEST(TextTest, Join)
{
    const auto empty = std::vector<int>{};
    const auto ints = std::vector{1, 2, 3};
    const auto strings = std::vector{"abc", "def", "юникод"};

    EXPECT_EQ(vsl::join(empty), "");

    EXPECT_EQ(vsl::join(ints), "1, 2, 3");
    EXPECT_EQ(vsl::join(ints, "+"), "1+2+3");
    EXPECT_EQ(vsl::join(ints, ""), "123");

    EXPECT_EQ(vsl::join(strings), "abc, def, юникод");
    EXPECT_EQ(vsl::join(strings, "_"), "abc_def_юникод");
    EXPECT_EQ(vsl::join(strings, ""), "abcdefюникод");

    EXPECT_EQ(vsl::join(ints, "+", "{:02}"), "01+02+03");
    EXPECT_EQ(vsl::join(strings, "_", "{:?}"), R"("abc"_"def"_"юникод")");
}

static const auto R = std::string{"\r"};
static const auto N = std::string{"\n"};
static const auto RN = std::string{"\r\n"};

TEST(TextTest, Indent)
{
    auto test_indent = [](auto&& str, int width, auto&& expected_res)
    {
        auto res = std::string{};
        res.reserve(100);
        vsl::indent(res, str, width);
        EXPECT_EQ(res, expected_res);

        EXPECT_EQ(vsl::indent(str, width), expected_res);
    };

    test_indent("", 2, "");

    test_indent(R, 2, R);
    test_indent(N, 2, N);
    test_indent(RN, 2, RN);

    test_indent(R + R, 2, R + R);
    test_indent(N + N, 2, N + N);
    test_indent(RN + RN, 2, RN + RN);

    test_indent(" ", 2, "   ");
    test_indent(" " + RN + " ", 2, "   " + RN + "   ");

    test_indent("юникод", 1, " юникод");
    test_indent("юникод", 0, "юникод");
    test_indent("юникод", -1, "юникод");

    test_indent(R + "юникод", 2, R + "  юникод");
    test_indent(N + "юникод", 2, N + "  юникод");
    test_indent(RN + "юникод", 2, RN + "  юникод");

    test_indent("юникод" + R, 2, "  юникод" + R);
    test_indent("юникод" + N, 2, "  юникод" + N);
    test_indent("юникод" + RN, 2, "  юникод" + RN);

    test_indent(R + "юникод" + R, 2, R + "  юникод" + R);
    test_indent(N + "юникод" + N, 2, N + "  юникод" + N);
    test_indent(RN + "юникод" + RN, 2, RN + "  юникод" + RN);

    test_indent("раз" + R + "два", 2, "  раз" + R + "  два");
    test_indent("раз" + N + "два", 2, "  раз" + N + "  два");
    test_indent("раз" + RN + "два", 2, "  раз" + RN + "  два");

    test_indent(N + R + RN + "раз" + N + R + RN + "два" + N + R + RN, 2,
                N + R + RN + "  раз" + N + R + RN + "  два" + N + R + RN);
}

TEST(TextTest, ToLf)
{
    auto test_to_lf = [](auto&& str, auto&& expected_res) { EXPECT_EQ(vsl::to_lf(str), expected_res); };

    test_to_lf("", "");

    test_to_lf(R, N);
    test_to_lf(N, N);
    test_to_lf(RN, N);

    test_to_lf(R + "юникод", N + "юникод");
    test_to_lf(N + "юникод", N + "юникод");
    test_to_lf(RN + "юникод", N + "юникод");

    test_to_lf("юникод" + R, "юникод" + N);
    test_to_lf("юникод" + N, "юникод" + N);
    test_to_lf("юникод" + RN, "юникод" + N);

    test_to_lf(R + "юникод" + R, N + "юникод" + N);
    test_to_lf(N + "юникод" + N, N + "юникод" + N);
    test_to_lf(RN + "юникод" + RN, N + "юникод" + N);

    test_to_lf("раз" + R + "два", "раз" + N + "два");
    test_to_lf("раз" + N + "два", "раз" + N + "два");
    test_to_lf("раз" + RN + "два", "раз" + N + "два");

    test_to_lf(N + R + RN + "раз" + N + R + RN + "два" + N + R + RN, N + N + N + "раз" + N + N + N + "два" + N + N + N);
}

TEST(TextTest, ToCrlf)
{
    auto test_to_crlf = [](auto&& str, auto&& expected_res)
    {
        auto res = std::string{};
        res.reserve(100);
        vsl::to_crlf(res, str);
        EXPECT_EQ(res, expected_res);

        EXPECT_EQ(vsl::to_crlf(str), expected_res);
    };

    test_to_crlf("", "");

    test_to_crlf(R, RN);
    test_to_crlf(N, RN);
    test_to_crlf(RN, RN);

    test_to_crlf(R + "юникод", RN + "юникод");
    test_to_crlf(N + "юникод", RN + "юникод");
    test_to_crlf(RN + "юникод", RN + "юникод");

    test_to_crlf("юникод" + R, "юникод" + RN);
    test_to_crlf("юникод" + N, "юникод" + RN);
    test_to_crlf("юникод" + RN, "юникод" + RN);

    test_to_crlf(R + "юникод" + R, RN + "юникод" + RN);
    test_to_crlf(N + "юникод" + N, RN + "юникод" + RN);
    test_to_crlf(RN + "юникод" + RN, RN + "юникод" + RN);

    test_to_crlf("раз" + R + "два", "раз" + RN + "два");
    test_to_crlf("раз" + N + "два", "раз" + RN + "два");
    test_to_crlf("раз" + RN + "два", "раз" + RN + "два");

    test_to_crlf(N + R + RN + "раз" + N + R + RN + "два" + N + R + RN,
                 RN + RN + RN + "раз" + RN + RN + RN + "два" + RN + RN + RN);
}

TEST(TextTest, FormatCountOfTotal)
{
    EXPECT_EQ(vsl::format_count_of_total(0, 0), "0");
    EXPECT_EQ(vsl::format_count_of_total(3, 3), "3");
    EXPECT_EQ(vsl::format_count_of_total(3, 4), "3/4");
    EXPECT_EQ(vsl::format_count_of_total(i16{3}, u64{4}), "3/4");

    EXPECT_EQ(vsl::format_count_of_total(10, 20, " из "), "10 из 20");
    EXPECT_EQ(vsl::format_count_of_total(20, 20, " из "), "20");
}

}  // namespace test
