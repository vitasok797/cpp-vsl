#include "text.h"

#include <vsl/concepts.h>
#include <vsl/types.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
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
    EXPECT_EQ(vsl::to_upper("", vsl::ascii), "");
    EXPECT_EQ(vsl::to_lower("", vsl::ascii), "");

    EXPECT_EQ(vsl::to_upper(" \n 123 ", vsl::ascii), " \n 123 ");
    EXPECT_EQ(vsl::to_lower(" \n 123 ", vsl::ascii), " \n 123 ");

    EXPECT_EQ(vsl::to_upper("az AZ az", vsl::ascii), "AZ AZ AZ");
    EXPECT_EQ(vsl::to_lower("az AZ az", vsl::ascii), "az az az");

    EXPECT_EQ(vsl::to_upper("ЮНИкод", vsl::ascii), "ЮНИкод");
    EXPECT_EQ(vsl::to_lower("ЮНИкод", vsl::ascii), "ЮНИкод");
}

TEST(TextTest, IsEqual)
{
    EXPECT_TRUE(vsl::is_equal("юникод", "юникод"));
    EXPECT_FALSE(vsl::is_equal("ЮНИКОД", "юникод"));

    EXPECT_TRUE(vsl::is_equal("ﬃ", "ﬃ"));
    EXPECT_FALSE(vsl::is_equal("ﬃ", "ffi"));
}

TEST(TextTest, IsEqualIgnoreCase)
{
    EXPECT_TRUE(vsl::is_equal("ЮНИКОД", "юникод", vsl::ignore_case));
    EXPECT_TRUE(vsl::is_equal("ﬃ", "ffi", vsl::ignore_case));
    EXPECT_TRUE(vsl::is_equal("Straße", "STRASSE", vsl::ignore_case));
    EXPECT_FALSE(vsl::is_equal("тест", "текст", vsl::ignore_case));
}

TEST(TextTest, IsEqualIgnoreAsciiCase)
{
    EXPECT_FALSE(vsl::is_equal("test", "test1", vsl::ignore_ascii_case));  // Different size

    EXPECT_TRUE(vsl::is_equal("", "", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::is_equal(" ", " ", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::is_equal("test", "test", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal("test", "text", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::is_equal("Test", "test", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal("Test", "xest", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::is_equal("teSt", "test", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal("teSt", "text", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::is_equal("tesT", "test", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal("tesT", "tesx", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::is_equal("TEST", "test", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal("TEST", "xxxx", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::is_equal("123", "123", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal("123", "124", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::is_equal("!@#$%", "!@#$%", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal("!@#$%", "!@#$_", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::is_equal("a1z", "a1z", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::is_equal("A1Z", "a1z", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal("a1z", "a2z", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::is_equal("a!z", "a!z", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::is_equal("A!Z", "a!z", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal("a!z", "a@z", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::is_equal("a\x01z", "a\x01z", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::is_equal("A\x01Z", "a\x01z", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal("a\x01z", "a\x02z", vsl::ignore_ascii_case));

    // ASCII letter range boundaries
    EXPECT_TRUE(vsl::is_equal("a", "A", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::is_equal("z", "Z", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::is_equal(std::string{'a' - 1}, std::string{'A' - 1}, vsl::ignore_ascii_case));  // '`' and '@'
    EXPECT_FALSE(vsl::is_equal(std::string{'z' + 1}, std::string{'Z' + 1}, vsl::ignore_ascii_case));  // '{' and '['

    // NOTE: Should be equal to the CHUNK_SIZE constant in the is_equal(..., IgnoreAsciiCaseTag) function
    constexpr auto CHUNK_SIZE = size_t{512};

    auto test_multichunk = [](bool expected_res, size_t len, char fill_char2,
                              std::string::size_type mismatch_pos = std::string::npos, char mismatch_char2 = 0)
    {
        auto str1 = std::string(len, 'a');
        auto str2 = std::string(len, fill_char2);
        if (mismatch_pos != std::string::npos)
        {
            str2[mismatch_pos] = mismatch_char2;
        }
        EXPECT_EQ(vsl::is_equal(str1, str2, vsl::ignore_ascii_case), expected_res);
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

        for (auto i = size_t{0}; i < len; ++i)
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

        EXPECT_TRUE(vsl::is_equal(str1, str2, vsl::ignore_ascii_case));
    }
}

TEST(TextTest, CompareStr)
{
    // TODO: Add vsl::compare_str tests
}

TEST(TextTest, CollateStr)
{
    EXPECT_EQ(vsl::collate_str("арбуз", "арбуз"), 0);
    EXPECT_GT(vsl::collate_str("Арбуз", "арбуз"), 0);
    EXPECT_LT(vsl::collate_str("арбуз", "Арбуз"), 0);

    EXPECT_EQ(vsl::collate_str("арбуз", "Арбуз", vsl::ignore_case), 0);
    EXPECT_EQ(vsl::collate_str("Арбуз", "арбуз", vsl::ignore_case), 0);
    EXPECT_EQ(vsl::collate_str("LIGATURE ﬃ", "ligature ffi", vsl::ignore_case), 0);

    auto vec = std::vector<std::string>{"браво", "астра", "АЛЬФА", "ОМЕГА"};
    std::ranges::sort(vec);
    EXPECT_THAT(vec, testing::ElementsAre("АЛЬФА", "ОМЕГА", "астра", "браво"));
    std::ranges::sort(vec, [](auto a, auto b) { return vsl::collate_str(a, b) < 0; });
    EXPECT_THAT(vec, testing::ElementsAre("астра", "АЛЬФА", "браво", "ОМЕГА"));
    std::ranges::sort(vec, [](auto a, auto b) { return vsl::collate_str(a, b, vsl::ignore_case) < 0; });
    EXPECT_THAT(vec, testing::ElementsAre("АЛЬФА", "астра", "браво", "ОМЕГА"));
}

TEST(TextTest, FindSubstr)
{
    // operator bool()
    const vsl::FoundSubstr found = vsl::find_substr("раз два три", "два");
    EXPECT_TRUE(found);

    // ignore_case
    EXPECT_TRUE(vsl::find_substr("раз два три", "два"));
    EXPECT_FALSE(vsl::find_substr("раз два три", "ДВА"));
    EXPECT_TRUE(vsl::find_substr("раз два три", "ДВА", vsl::ignore_case));

    // ignore_case
    EXPECT_TRUE(vsl::find_substr("Ligature ﬃ search", "ﬃ"));
    EXPECT_FALSE(vsl::find_substr("Ligature ﬃ search", "ffi"));
    EXPECT_TRUE(vsl::find_substr("Ligature ﬃ search", "ffi", vsl::ignore_case));

    // pos + ignore_case
    const vsl::FoundSubstr found2 = vsl::find_substr("Ligature ﬃ search", "ffi", vsl::ignore_case);
    EXPECT_EQ(found2.pos(), 9);
    EXPECT_EQ(found2.end_pos(), 12);
}

TEST(TextTest, StartsWithIgnoreAsciiCase)
{
    EXPECT_TRUE(vsl::starts_with("test", "te", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::starts_with("test", "TE", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::starts_with("test", "xx", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::starts_with("test", "testing", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::starts_with("", "", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::starts_with("abc", "", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::starts_with("", "abc", vsl::ignore_ascii_case));
}

TEST(TextTest, EndsWithIgnoreAsciiCase)
{
    EXPECT_TRUE(vsl::ends_with("test", "st", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::ends_with("test", "ST", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::ends_with("test", "xx", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::ends_with("ing", "testing", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::ends_with("", "", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::ends_with("abc", "", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::ends_with("", "abc", vsl::ignore_ascii_case));
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
    EXPECT_EQ(vsl::trim_start("", vsl::ascii), "");
    EXPECT_EQ(vsl::trim_end("", vsl::ascii), "");
    EXPECT_EQ(vsl::trim("", vsl::ascii), "");

    EXPECT_EQ(vsl::trim_start("abc", vsl::ascii), "abc");
    EXPECT_EQ(vsl::trim_end("abc", vsl::ascii), "abc");
    EXPECT_EQ(vsl::trim("abc", vsl::ascii), "abc");

    EXPECT_EQ(vsl::trim(WHITESPACES, vsl::ascii), "");
    EXPECT_EQ(vsl::trim(NBSP, vsl::ascii), NBSP);
    EXPECT_EQ(vsl::trim(NBSP + NBSP, vsl::ascii), NBSP + NBSP);

    EXPECT_EQ(vsl::trim_start(WHITESPACES + "юникод" + WHITESPACES, vsl::ascii), "юникод" + WHITESPACES);
    EXPECT_EQ(vsl::trim_end(WHITESPACES + "юникод" + WHITESPACES, vsl::ascii), WHITESPACES + "юникод");
    EXPECT_EQ(vsl::trim(WHITESPACES + "юникод" + WHITESPACES, vsl::ascii), "юникод");
    EXPECT_EQ(vsl::trim(WHITESPACES + NBSP + "юникод" + NBSP + WHITESPACES, vsl::ascii), NBSP + "юникод" + NBSP);

    const auto symbols = "$%";
    EXPECT_EQ(vsl::trim_start("$$%%$$  юникод  $$%%$$", vsl::ascii, symbols), "  юникод  $$%%$$");
    EXPECT_EQ(vsl::trim_end("$$%%$$  юникод  $$%%$$", vsl::ascii, symbols), "$$%%$$  юникод  ");
    EXPECT_EQ(vsl::trim("$$%%$$  юникод  $$%%$$", vsl::ascii, symbols), "  юникод  ");
}

TEST(TextTest, TrimPrefix)
{
    EXPECT_EQ(vsl::trim_prefix("test", "te"), "st");
    EXPECT_EQ(vsl::trim_prefix("test", "TE"), "test");
    EXPECT_EQ(vsl::trim_prefix("test", "testing"), "test");

    EXPECT_EQ(vsl::trim_prefix("", ""), "");
    EXPECT_EQ(vsl::trim_prefix("abc", ""), "abc");
    EXPECT_EQ(vsl::trim_prefix("", "abc"), "");

    EXPECT_EQ(vsl::trim_prefix("юникод", "юни"), "код");
}

TEST(TextTest, TrimSuffix)
{
    EXPECT_EQ(vsl::trim_suffix("test", "st"), "te");
    EXPECT_EQ(vsl::trim_suffix("test", "ST"), "test");
    EXPECT_EQ(vsl::trim_suffix("ing", "testing"), "ing");

    EXPECT_EQ(vsl::trim_suffix("", ""), "");
    EXPECT_EQ(vsl::trim_suffix("abc", ""), "abc");
    EXPECT_EQ(vsl::trim_suffix("", "abc"), "");

    EXPECT_EQ(vsl::trim_suffix("юникод", "код"), "юни");
}

TEST(TextTest, TrimPrefixIgnoreAsciiCase)
{
    EXPECT_EQ(vsl::trim_prefix("test", "te", vsl::ignore_ascii_case), "st");
    EXPECT_EQ(vsl::trim_prefix("test", "TE", vsl::ignore_ascii_case), "st");
    EXPECT_EQ(vsl::trim_prefix("test", "xx", vsl::ignore_ascii_case), "test");
    EXPECT_EQ(vsl::trim_prefix("test", "testing", vsl::ignore_ascii_case), "test");

    EXPECT_EQ(vsl::trim_prefix("", "", vsl::ignore_ascii_case), "");
    EXPECT_EQ(vsl::trim_prefix("abc", "", vsl::ignore_ascii_case), "abc");
    EXPECT_EQ(vsl::trim_prefix("", "abc", vsl::ignore_ascii_case), "");
}

TEST(TextTest, TrimSuffixIgnoreAsciiCase)
{
    EXPECT_EQ(vsl::trim_suffix("test", "st", vsl::ignore_ascii_case), "te");
    EXPECT_EQ(vsl::trim_suffix("test", "ST", vsl::ignore_ascii_case), "te");
    EXPECT_EQ(vsl::trim_suffix("test", "xx", vsl::ignore_ascii_case), "test");
    EXPECT_EQ(vsl::trim_suffix("ing", "testing", vsl::ignore_ascii_case), "ing");

    EXPECT_EQ(vsl::trim_suffix("", "", vsl::ignore_ascii_case), "");
    EXPECT_EQ(vsl::trim_suffix("abc", "", vsl::ignore_ascii_case), "abc");
    EXPECT_EQ(vsl::trim_suffix("", "abc", vsl::ignore_ascii_case), "");
}

TEST(TextTest, Split)
{
    auto test_split = [](auto&& s, auto&& sep, auto&& opt, auto&& expected_res,
                         vsl::Index max_tokens = std::numeric_limits<vsl::Index>::max())
    {
        using namespace testing;

        {
            auto res = std::vector<std::string>{};
            res.reserve(100);
            vsl::split(res, s, sep, opt, max_tokens);
            EXPECT_THAT(res, ElementsAreArray(expected_res));
        }
        {
            const auto res = vsl::split(s, sep, opt, max_tokens);
            static_assert(vsl::same_type_as<decltype(res), std::vector<std::string>>);
            EXPECT_THAT(res, ElementsAreArray(expected_res));
        }
        {
            auto res = std::vector<std::string_view>{};
            res.reserve(100);
            vsl::split(res, s, sep, opt, max_tokens);
            EXPECT_THAT(res, ElementsAreArray(expected_res));
        }
        {
            const auto res = vsl::split<std::string_view>(s, sep, opt, max_tokens);
            static_assert(vsl::same_type_as<decltype(res), std::vector<std::string_view>>);
            EXPECT_THAT(res, ElementsAreArray(expected_res));
        }
    };

    const auto sep = ";";
    const auto no_opt = vsl::SplitOptions::NONE;
    const auto empty_res = std::vector<std::string_view>{};

    {
        const auto s = "";
        const auto expected_res = {""};  // NOTE: The result is different for re_split/split
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
        const auto expected_res = {"", ""};  // NOTE: The result is different for re_split/split
        test_split(s, sep, no_opt, expected_res);
    }
    {
        const auto s = ";;";
        const auto expected_res = {"", "", ""};  // NOTE: The result is different for re_split/split
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
        const auto expected_res = {"", "1", "", "3", ""};  // NOTE: The result is different for re_split/split
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

    // Max token count

    // NOTE: When 'max_tokens' is reached, the last token takes the entire remaining string.
    // TRIM and SKIP_EMPTY options are applied to it as usual.

    {
        const auto s = "1;2;3";
        const auto max_tokens = 99;
        const auto expected_res = {"1", "2", "3"};
        test_split(s, sep, no_opt, expected_res, max_tokens);
    }
    {
        const auto s = "1;2;3";
        const auto max_tokens = 3;
        const auto expected_res = {"1", "2", "3"};
        test_split(s, sep, no_opt, expected_res, max_tokens);
    }
    {
        const auto s = "1;2;3";
        const auto max_tokens = 2;
        const auto expected_res = {"1", "2;3"};
        test_split(s, sep, no_opt, expected_res, max_tokens);
    }
    {
        const auto s = "1;2;3";
        const auto max_tokens = 1;
        const auto expected_res = {"1;2;3"};
        test_split(s, sep, no_opt, expected_res, max_tokens);
    }
    {
        const auto s = "1;2;3";
        const auto max_tokens = 0;
        const auto expected_res = empty_res;
        test_split(s, sep, no_opt, expected_res, max_tokens);
    }
    {
        const auto s = "1;;2;;;3";
        const auto max_tokens = 3;
        const auto expected_res = {"1", "", "2;;;3"};
        test_split(s, sep, no_opt, expected_res, max_tokens);
    }
    {
        const auto s = "1;;2;;;3";
        const auto opt = vsl::SplitOptions::SKIP_EMPTY;
        const auto max_tokens = 3;
        const auto expected_res = {"1", "2", ";;3"};
        test_split(s, sep, opt, expected_res, max_tokens);
    }
    {
        const auto s = "1;2;3;;";
        const auto opt = vsl::SplitOptions::SKIP_EMPTY;
        const auto max_tokens = 3;
        const auto expected_res = {"1", "2", "3;;"};
        test_split(s, sep, opt, expected_res, max_tokens);
    }
    {
        const auto s = "1;2;3;;";
        const auto opt = vsl::SplitOptions::SKIP_EMPTY;
        const auto max_tokens = 4;
        const auto expected_res = {"1", "2", "3", ";"};
        test_split(s, sep, opt, expected_res, max_tokens);
    }
    {
        const auto s = "1;2; 3; ; ";
        const auto opt = vsl::SplitOptions::TRIM | vsl::SplitOptions::SKIP_EMPTY;
        const auto max_tokens = 3;
        const auto expected_res = {"1", "2", "3; ;"};
        test_split(s, sep, opt, expected_res, max_tokens);
    }
}

TEST(TextTest, Replace)
{
    EXPECT_EQ(vsl::replace("", "abc", "_"), "");
    EXPECT_EQ(vsl::replace("abc", "", "_"), "abc");

    EXPECT_EQ(vsl::replace("123", "abc", "_"), "123");
    EXPECT_EQ(vsl::replace("ab", "abc", "_"), "ab");

    EXPECT_EQ(vsl::replace("abc", "abc", "_"), "_");
    EXPECT_EQ(vsl::replace("123abc456", "abc", "_"), "123_456");
    EXPECT_EQ(vsl::replace("abc123abc", "abc", "_"), "_123_");
    EXPECT_EQ(vsl::replace("aaaaaa", "aaa", "_"), "__");

    EXPECT_EQ(vsl::replace("abc123abc", "ABC", "_"), "abc123abc");
    EXPECT_EQ(vsl::replace("abc123abc", "ABC", "_", vsl::ignore_ascii_case), "_123_");
    EXPECT_EQ(vsl::replace("abc123abc", "DEF", "_", vsl::ignore_ascii_case), "abc123abc");

    EXPECT_EQ(vsl::replace("abc123abc456abc", "abc", "_", 0), "abc123abc456abc");
    EXPECT_EQ(vsl::replace("abc123abc456abc", "abc", "_", 1), "_123abc456abc");
    EXPECT_EQ(vsl::replace("abc123abc456abc", "abc", "_", 2), "_123_456abc");
    EXPECT_EQ(vsl::replace("abc123abc456abc", "abc", "_", 3), "_123_456_");
    EXPECT_EQ(vsl::replace("abc123abc456abc", "abc", "_", 99), "_123_456_");

    auto repl_func = [](auto&& sv) { return std::string{"["}.append(sv).append("]"); };
    EXPECT_EQ(vsl::replace("abc123abc456abc", "abc", repl_func), "[abc]123[abc]456[abc]");
    EXPECT_EQ(vsl::replace("abc123abc456abc", "abc", repl_func, 2), "[abc]123[abc]456abc");
    EXPECT_EQ(vsl::replace("abc123abc456abc", "ABC", repl_func, vsl::ignore_ascii_case), "[abc]123[abc]456[abc]");
    EXPECT_EQ(vsl::replace("abc123abc456abc", "ABC", repl_func, vsl::ignore_ascii_case, 2), "[abc]123[abc]456abc");
}

TEST(TextTest, ContainsSubstr)
{
    EXPECT_TRUE(vsl::contains_substr("", ""));
    EXPECT_TRUE(vsl::contains_substr("abc", ""));
    EXPECT_FALSE(vsl::contains_substr("", "abc"));

    EXPECT_TRUE(vsl::contains_substr("", "", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::contains_substr("abc", "", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::contains_substr("", "abc", vsl::ignore_ascii_case));

    EXPECT_TRUE(vsl::contains_substr("юникод", "юникод"));
    EXPECT_TRUE(vsl::contains_substr("юникод123", "юникод"));
    EXPECT_TRUE(vsl::contains_substr("123юникод", "юникод"));
    EXPECT_TRUE(vsl::contains_substr("123юникод123", "юникод"));
    EXPECT_FALSE(vsl::contains_substr("abcd", "test"));
    EXPECT_FALSE(vsl::contains_substr("tes", "test"));

    EXPECT_TRUE(vsl::contains_substr("test", "test", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::contains_substr("test123", "test", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::contains_substr("123test", "test", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::contains_substr("123test123", "test", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::contains_substr("abcd", "test", vsl::ignore_ascii_case));
    EXPECT_FALSE(vsl::contains_substr("tes", "test", vsl::ignore_ascii_case));

    EXPECT_FALSE(vsl::contains_substr("test", "TEST"));
    EXPECT_TRUE(vsl::contains_substr("test", "TEST", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::contains_substr("test123", "TEST", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::contains_substr("123test", "TEST", vsl::ignore_ascii_case));
    EXPECT_TRUE(vsl::contains_substr("123test123", "TEST", vsl::ignore_ascii_case));
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

TEST(TextTest, RepeatStr)
{
    EXPECT_EQ(vsl::repeat_str("abc", -1), "");
    EXPECT_EQ(vsl::repeat_str("abc", 0), "");
    EXPECT_EQ(vsl::repeat_str("abc", 1), "abc");
    EXPECT_EQ(vsl::repeat_str("abc", 2), "abcabc");
    EXPECT_EQ(vsl::repeat_str("", 1), "");

    EXPECT_THROW([[maybe_unused]] auto res = vsl::repeat_str("12345", std::numeric_limits<vsl::Index>::max() / 4),
                 std::length_error);
}

}  // namespace test
