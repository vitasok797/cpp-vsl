#include "text.h"

#include <vsl/types.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

namespace vsl::test
{

TEST(TextTest, Case)
{
    EXPECT_EQ(vsl::to_casefold("LIGATURE ﬃ"), "ligature ffi");
    EXPECT_EQ(vsl::to_lowercase("LIGATURE ﬃ"), "ligature ﬃ");
    EXPECT_EQ(vsl::to_lowercase("ДВА"), "два");
    EXPECT_EQ(vsl::to_uppercase("Straße"), "STRASSE");
    EXPECT_EQ(vsl::to_titlecase("teMPuS eDAX reRuM"), "Tempus Edax Rerum");
}

TEST(TextTest, CompareStr)
{
    EXPECT_TRUE(vsl::compare_str("тест", "тест"));
    EXPECT_FALSE(vsl::compare_str("ТЕСТ", "тест"));
    EXPECT_TRUE(vsl::compare_str_icase("ТЕСТ", "тест"));

    EXPECT_TRUE(vsl::compare_str("ﬃ", "ﬃ"));
    EXPECT_FALSE(vsl::compare_str("ﬃ", "ffi"));
    EXPECT_TRUE(vsl::compare_str_icase("ﬃ", "ffi"));

    EXPECT_TRUE(vsl::compare_str_icase("Straße", "STRASSE"));
    EXPECT_FALSE(vsl::compare_str_icase("тест", "текст"));
}

TEST(TextTest, Collate)
{
    EXPECT_EQ(vsl::collate("арбуз", "арбуз"), 0);
    EXPECT_GT(vsl::collate("Арбуз", "арбуз"), 0);
    EXPECT_LT(vsl::collate("арбуз", "Арбуз"), 0);

    EXPECT_EQ(vsl::collate_icase("арбуз", "Арбуз"), 0);
    EXPECT_EQ(vsl::collate_icase("Арбуз", "арбуз"), 0);
    EXPECT_EQ(vsl::collate_icase("LIGATURE ﬃ", "ligature ffi"), 0);

    auto vec = std::vector<std::string>{"браво", "астра", "АЛЬФА", "ОМЕГА"};
    std::ranges::sort(vec);
    EXPECT_THAT(vec, testing::ElementsAre("АЛЬФА", "ОМЕГА", "астра", "браво"));
    std::ranges::sort(vec, [](auto a, auto b) { return vsl::collate(a, b) < 0; });
    EXPECT_THAT(vec, testing::ElementsAre("астра", "АЛЬФА", "браво", "ОМЕГА"));
    std::ranges::sort(vec, [](auto a, auto b) { return vsl::collate_icase(a, b) < 0; });
    EXPECT_THAT(vec, testing::ElementsAre("АЛЬФА", "астра", "браво", "ОМЕГА"));
}

TEST(TextTest, FindStr)
{
    EXPECT_TRUE(vsl::find_str("раз два три", "два"));
    EXPECT_FALSE(vsl::find_str("раз два три", "ДВА"));
    EXPECT_TRUE(vsl::find_str_icase("раз два три", "ДВА"));

    EXPECT_TRUE(vsl::find_str("Ligature ﬃ search", "ﬃ"));
    EXPECT_FALSE(vsl::find_str("Ligature ﬃ search", "ffi"));
    EXPECT_TRUE(vsl::find_str_icase("Ligature ﬃ search", "ffi"));

    const FoundStr found = vsl::find_str_icase("Ligature ﬃ search", "ffi");
    EXPECT_EQ(found.pos(), 9);
    EXPECT_EQ(found.end_pos(), 12);
}

TEST(TextTest, Trim)
{
    EXPECT_EQ(vsl::trim_left(""), "");
    EXPECT_EQ(vsl::trim_right(""), "");
    EXPECT_EQ(vsl::trim(""), "");

    EXPECT_EQ(vsl::trim_left(" \t "), "");
    EXPECT_EQ(vsl::trim_right(" \t "), "");
    EXPECT_EQ(vsl::trim(" \t "), "");

    EXPECT_EQ(vsl::trim_left("abc"), "abc");
    EXPECT_EQ(vsl::trim_right("abc"), "abc");
    EXPECT_EQ(vsl::trim("abc"), "abc");

    EXPECT_EQ(vsl::trim_left(" \t\n Тест"), "Тест");
    EXPECT_EQ(vsl::trim_right("Тест \t\n "), "Тест");
    EXPECT_EQ(vsl::trim(" \t\n Тест \t\n "), "Тест");

    EXPECT_EQ(vsl::trim_left("  abc  "), "abc  ");
    EXPECT_EQ(vsl::trim_right("  abc  "), "  abc");
}

TEST(TextTest, Join)
{
    const auto empty = std::vector<int>{};
    const auto ints = std::vector{1, 2, 3};
    const auto strings = std::vector{"abc", "def", "тест"};

    EXPECT_EQ(vsl::join(empty), "");

    EXPECT_EQ(vsl::join(ints), "1, 2, 3");
    EXPECT_EQ(vsl::join(ints, "+"), "1+2+3");

    EXPECT_EQ(vsl::join_fmt(ints, ":02"), "01, 02, 03");
    EXPECT_EQ(vsl::join_fmt(ints, ":02", "+"), "01+02+03");

    EXPECT_EQ(vsl::join(strings), "abc, def, тест");
    EXPECT_EQ(vsl::join(strings, "_"), "abc_def_тест");

    EXPECT_EQ(vsl::join_fmt(strings, ":?"), R"("abc", "def", "тест")");
    EXPECT_EQ(vsl::join_fmt(strings, ":?", "_"), R"("abc"_"def"_"тест")");
}

TEST(TextTest, Indent)
{
    EXPECT_EQ(vsl::indent("AAA\nBBB\nCCC", 2), "  AAA\n  BBB\n  CCC");
    EXPECT_EQ(vsl::indent("AAA\nBBB\nCCC\n", 2), "  AAA\n  BBB\n  CCC\n");
    EXPECT_EQ(vsl::indent("AAA\r\nBBB\r\nCCC", 2), "  AAA\r\n  BBB\r\n  CCC");
    EXPECT_EQ(vsl::indent("AAA\nBBB\nCCC", 0), "AAA\nBBB\nCCC");
    EXPECT_EQ(vsl::indent("AAA\nBBB\nCCC", -1), "AAA\nBBB\nCCC");
    EXPECT_EQ(vsl::indent("\n\nAAA\nBBB\n\n\nCCC\n\n", 1), "\n\n AAA\n BBB\n\n\n CCC\n\n");
    EXPECT_EQ(vsl::indent("", 1), "");
    EXPECT_EQ(vsl::indent(" ", 1), "  ");
    EXPECT_EQ(vsl::indent(" \n ", 1), "  \n  ");
    EXPECT_EQ(vsl::indent("AAA", 2), "  AAA");
    EXPECT_EQ(vsl::indent("раз\nдва\nтри", 2), "  раз\n  два\n  три");
    EXPECT_EQ(vsl::indent("раз\r\nдва\r\nтри", 2), "  раз\r\n  два\r\n  три");
}

TEST(TextTest, LineEndings)
{
    const auto N = std::string{"\n"};
    const auto RN = std::string{"\r\n"};

    EXPECT_EQ(to_lf(""), "");
    EXPECT_EQ(to_crlf(""), "");

    EXPECT_EQ(to_lf(RN), N);
    EXPECT_EQ(to_crlf(N), RN);

    EXPECT_EQ(to_lf(N), N);
    EXPECT_EQ(to_crlf(RN), RN);

    EXPECT_EQ(to_lf(N + N + RN + N), (N + N + N + N));
    EXPECT_EQ(to_crlf(RN + RN + N + RN), (RN + RN + RN + RN));

    EXPECT_EQ(to_lf("abc" + RN + "def" + N + "тест" + RN + "123" + RN),
              ("abc" + N + "def" + N + "тест" + N + "123" + N));
    EXPECT_EQ(to_crlf("abc" + N + "def" + N + "тест" + RN + "123" + N),
              ("abc" + RN + "def" + RN + "тест" + RN + "123" + RN));
}

TEST(TextTest, OutOf)
{
    EXPECT_EQ(vsl::out_of(3, 3), "3");
    EXPECT_EQ(vsl::out_of(3, 4), "3/4");
    EXPECT_EQ(vsl::out_of(i16{3}, u64{4}), "3/4");
}

}  // namespace vsl::test
