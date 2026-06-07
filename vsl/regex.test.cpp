#include "regex.h"

#include <vsl/concepts.h>
#include <vsl/types.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace test
{

TEST(RegexTest, Create)
{
    auto test_good_pattern = [](auto&& pattern)
    {
        EXPECT_NO_THROW(auto re = vsl::Re(pattern));
        EXPECT_NO_THROW(auto re = vsl::ReAscii(pattern));

        const auto flags = vsl::Re::multiline | vsl::Re::dotall | vsl::Re::icase | vsl::Re::nosubs | vsl::Re::optimize;
        EXPECT_NO_THROW(auto re = vsl::Re(pattern, flags));
        EXPECT_NO_THROW(auto re = vsl::ReAscii(pattern, flags));
    };

    const auto good_pattern = "\\d+";
    test_good_pattern(good_pattern);
    test_good_pattern(std::string{good_pattern});

    EXPECT_THROW(auto re = vsl::Re("+"), vsl::ReError);
    EXPECT_THROW(auto re = vsl::ReAscii("[]]"), vsl::ReError);
}

TEST(RegexTest, FullMatch)
{
    auto test_full_match = [](auto&& s, auto&& re, vsl::ReMatch* match, vsl::ReMatchFlags flags, bool expected_status)
    {
        const auto success = [&]
        {
            if (match)
            {
                return vsl::re_full_match(s, re, *match, flags);
            }
            else
            {
                return vsl::re_full_match(s, re, flags);
            }
        }();

        if (expected_status)
        {
            EXPECT_TRUE(success);
            if (match)
            {
                EXPECT_EQ(match->str(), s);
                EXPECT_FALSE(match->empty());
                EXPECT_TRUE(match->size() > 0);
            }
        }
        else
        {
            EXPECT_FALSE(success);
            if (match)
            {
                EXPECT_TRUE(match->empty());
                EXPECT_EQ(match->size(), 0);
            }
        }
    };

    const auto s = "123abc456";
    const auto pattern = "\\d+\\D+\\d+";
    const auto re = vsl::Re{pattern};
    auto match = vsl::ReMatch{};
    const auto no_flags = vsl::ReMatchFlags::DEFAULT;

    // arg: base
    test_full_match(s, re, &match, no_flags, true);

    // arg: s
    test_full_match(std::string{s}, re, &match, no_flags, true);
    test_full_match(std::string_view{s}, re, &match, no_flags, true);

    // arg: re
    test_full_match(s, vsl::ReAscii{pattern}, &match, no_flags, true);

    // arg: match
    test_full_match(s, re, nullptr, no_flags, true);

    // arg: flags
    test_full_match("", vsl::Re{"\\d*"}, &match, no_flags, true);
    test_full_match("", vsl::Re{"\\d*"}, &match, vsl::ReMatchFlags::MATCH_NOT_NULL, false);

    // no match found
    test_full_match(s, vsl::Re{"\\d+"}, &match, no_flags, false);
}

TEST(RegexTest, FullMatchUseSubmatches)
{
    const auto str = "123abc456";
    const auto re = vsl::Re{"\\d+(\\D+)\\d+|(\\D+)"};

    auto match = vsl::ReMatch{};
    EXPECT_FALSE(match.ready());

    EXPECT_TRUE(vsl::re_full_match(str, re, match));

    EXPECT_TRUE(match.ready());
    EXPECT_FALSE(match.empty());
    EXPECT_EQ(match.size(), 3);

    EXPECT_TRUE(match[0].matched);
    EXPECT_TRUE(match[1].matched);
    EXPECT_FALSE(match[2].matched);

    EXPECT_EQ(std::string(match[0].first, match[0].second), str);
    EXPECT_EQ(std::string(match[1].first, match[1].second), "abc");

    EXPECT_EQ(match.str(), str);
    EXPECT_EQ(match.str(0), str);
    EXPECT_EQ(match.str(1), "abc");
    EXPECT_EQ(match[0].str(), str);
    EXPECT_EQ(match[1].str(), "abc");
    EXPECT_EQ(match[0], str);    // <=> operator
    EXPECT_EQ(match[1], "abc");  // <=> operator

    EXPECT_EQ(match.length(), 9);
    EXPECT_EQ(match.length(0), 9);
    EXPECT_EQ(match.length(1), 3);
    EXPECT_EQ(match[0].length(), 9);
    EXPECT_EQ(match[1].length(), 3);

    EXPECT_EQ(match.position(), 0);
    EXPECT_EQ(match.position(0), 0);
    EXPECT_EQ(match.position(1), 3);
}

TEST(RegexTest, FullMatchOnlyFull)
{
    const auto re = vsl::Re{"\\d+\\D+\\d+"};

    EXPECT_TRUE(vsl::re_full_match("123abc456", re));
    EXPECT_FALSE(vsl::re_full_match("_123abc456", re));
    EXPECT_FALSE(vsl::re_full_match("123abc456_", re));
}

TEST(RegexTest, Search)
{
    auto test_search = [](auto&& s, auto&& re, vsl::ReMatch* match, vsl::ReMatchFlags flags, vsl::cstring expected_res)
    {
        const auto success = [&]
        {
            if (match)
            {
                return vsl::re_search(s, re, *match, flags);
            }
            else
            {
                return vsl::re_search(s, re, flags);
            }
        }();

        if (expected_res)
        {
            EXPECT_TRUE(success);
            if (match)
            {
                EXPECT_EQ(match->str(), expected_res);
                EXPECT_FALSE(match->empty());
                EXPECT_TRUE(match->size() > 0);
            }
        }
        else
        {
            EXPECT_FALSE(success);
            if (match)
            {
                EXPECT_TRUE(match->empty());
                EXPECT_EQ(match->size(), 0);
            }
        }
    };

    const auto s = "123abc456";
    const auto pattern = "\\D+";
    const auto re = vsl::Re{pattern};
    auto match = vsl::ReMatch{};
    const auto no_flags = vsl::ReMatchFlags::DEFAULT;
    const auto expected_res = "abc";

    // arg: base
    test_search(s, re, &match, no_flags, expected_res);

    // arg: s
    test_search(std::string{s}, re, &match, no_flags, expected_res);
    test_search(std::string_view{s}, re, &match, no_flags, expected_res);

    // arg: re
    test_search(s, vsl::ReAscii{pattern}, &match, no_flags, expected_res);

    // arg: match
    test_search(s, re, nullptr, no_flags, expected_res);

    // arg: flags
    test_search(s, re, &match, vsl::ReMatchFlags::MATCH_CONTINUOUS, nullptr);
    test_search(s, vsl::Re{"\\d{3}\\D"}, &match, vsl::ReMatchFlags::MATCH_CONTINUOUS, "123a");

    // no match found
    test_search(s, vsl::Re{"\\D{4}"}, &match, no_flags, nullptr);
}

TEST(RegexTest, SearchUseSubmatches)
{
    const auto re = vsl::Re{"\\D(\\D+)"};

    EXPECT_FALSE(vsl::re_search("123000456", re));

    const auto str = std::string{"123abc456"};
    auto match = vsl::ReMatch{};
    EXPECT_FALSE(match.ready());

    EXPECT_TRUE(vsl::re_search(str, re, match));

    EXPECT_TRUE(match.ready());
    EXPECT_FALSE(match.empty());
    EXPECT_EQ(match.size(), 2);

    EXPECT_TRUE(match[0].matched);
    EXPECT_TRUE(match[1].matched);

    EXPECT_EQ(std::string(match[0].first, match[0].second), "abc");
    EXPECT_EQ(std::string(match[1].first, match[1].second), "bc");

    EXPECT_EQ(match.str(), "abc");
    EXPECT_EQ(match.str(0), "abc");
    EXPECT_EQ(match.str(1), "bc");
    EXPECT_EQ(match[0].str(), "abc");
    EXPECT_EQ(match[1].str(), "bc");
    EXPECT_EQ(match[0], "abc");  // <=> operator
    EXPECT_EQ(match[1], "bc");   // <=> operator

    EXPECT_EQ(match.length(), 3);
    EXPECT_EQ(match.length(0), 3);
    EXPECT_EQ(match.length(1), 2);
    EXPECT_EQ(match[0].length(), 3);
    EXPECT_EQ(match[1].length(), 2);

    EXPECT_EQ(match.position(), 3);
    EXPECT_EQ(match.position(0), 3);
    EXPECT_EQ(match.position(1), 4);

    EXPECT_EQ(match.prefix().str(), "123");
    EXPECT_EQ(match.suffix().str(), "456");
}

TEST(RegexTest, Find)
{
    // Ensure that the regex object passed to the re_find_matches/re_find_matches_sv/re_find_submatches
    // function outlives the returned range (intenal iterator stores a reference to the regex)

    auto test_find = [](auto&& s, auto&& re, vsl::ReMatchFlags flags, const std::vector<vsl::cstring>& expected_res)
    {
        {
            const auto res = vsl::re_find_matches(s, re, flags);
            static_assert(vsl::range_of<decltype(res), vsl::ReMatch>);
            const auto res_str = res | std::views::transform([](auto&& m) { return m.str(); });
            EXPECT_THAT(std::vector(res_str.begin(), res_str.end()), testing::ElementsAreArray(expected_res));
        }
        {
            const auto res = vsl::re_find_matches_sv(s, re, flags);
            static_assert(vsl::range_of<decltype(res), std::string_view>);
            EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
        }
        {
            const auto res = vsl::re_find_submatches(s, re, 0, flags);
            static_assert(vsl::range_of<decltype(res), std::string_view>);
            EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
        }
    };

    const auto s = "abc def xyz";
    const auto pattern = "[a-z].[a-z]";
    const auto re = vsl::Re{pattern};
    const auto no_flags = vsl::ReMatchFlags::DEFAULT;
    const auto expected_res = {"abc", "def", "xyz"};

    // arg: base
    test_find(s, re, no_flags, expected_res);

    // arg: s
    test_find(std::string{s}, re, no_flags, expected_res);
    test_find(std::string_view{s}, re, no_flags, expected_res);

    // arg: re
    test_find(s, vsl::ReAscii{pattern}, no_flags, expected_res);

    // arg: flags
    test_find(s, re, vsl::ReMatchFlags::MATCH_CONTINUOUS, {"abc"});

    // utf-8
    const auto s_utf8 = std::string_view{"первый второй третий"};
    const auto pattern_utf8 = "(?:пе.вый|тр.тий)";
    const auto re_utf8 = vsl::Re{pattern_utf8};
    const auto re_ascii = vsl::ReAscii{pattern_utf8};
    test_find(s_utf8, re_utf8, no_flags, {"первый", "третий"});
    test_find(s_utf8, re_ascii, no_flags, {});
}

TEST(RegexTest, FindSubmatches)
{
    // Ensure that the regex object passed to the re_find_submatches
    // function outlives the returned range (intenal iterator stores a reference to the regex)

    const auto s = "1abc2def3";
    const auto pattern = "[a-z]([a-z])[a-z]";
    const auto re = vsl::Re{pattern};
    const auto no_flags = vsl::ReMatchFlags::DEFAULT;
    const auto empty_res = std::vector<std::string_view>{};

    {
        const auto s2 = "";
        const auto submatches = -1;
        const auto expected_res = empty_res;
        const auto res = vsl::re_find_submatches(s2, re, submatches, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        // int submatches arg
        const auto submatches = -1;
        const auto expected_res = {"1", "2", "3"};
        const auto res = vsl::re_find_submatches(s, re, submatches, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        // int submatches arg
        const auto submatches = 0;
        const auto expected_res = {"abc", "def"};
        const auto res = vsl::re_find_submatches(s, re, submatches, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        // int submatches arg
        const auto submatches = 1;
        const auto expected_res = {"b", "e"};
        const auto res = vsl::re_find_submatches(s, re, submatches, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        // int submatches arg
        const auto submatches = 2;
        const auto expected_res = {"", ""};
        const auto res = vsl::re_find_submatches(s, re, submatches, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        // std::vector submatches arg
        const auto submatches = std::vector{-1, 0, 1};
        const auto expected_res = {"1", "abc", "b", "2", "def", "e", "3"};
        const auto res = vsl::re_find_submatches(s, re, submatches, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        // std::initializer_list submatches arg
        const auto expected_res = {"1", "b", "2", "e", "3"};
        const auto res = vsl::re_find_submatches(s, re, {-1, 1}, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s2 = "111";
        const auto submatches = -1;
        const auto expected_res = {"111"};
        const auto res = vsl::re_find_submatches(s2, re, submatches, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
}

TEST(RegexTest, Split)
{
    // Ensure that the regex object passed to the re_split
    // function outlives the returned range (intenal iterator stores a reference to the regex)

    const auto pattern = ",";
    const auto re = vsl::Re{pattern};
    const auto no_opt = vsl::ReSplitOptions::NONE;
    const auto no_flags = vsl::ReMatchFlags::DEFAULT;
    const auto empty_res = std::vector<std::string_view>{};

    {
        const auto s = "";
        const auto expected_res = empty_res;
        auto res = vsl::re_split(s, re, no_opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = "1,2,3";
        const auto expected_res = {"1", "2", "3"};
        auto res = vsl::re_split(s, re, no_opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = " 1, 2, 3 \n";
        const auto expected_res = {" 1", " 2", " 3 \n"};
        auto res = vsl::re_split(s, re, no_opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = " 1, 2, 3 \n";
        const auto opt = vsl::ReSplitOptions::TRIM;
        const auto expected_res = {"1", "2", "3"};
        auto res = vsl::re_split(s, re, opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = "1,,3";
        const auto expected_res = {"1", "", "3"};
        auto res = vsl::re_split(s, re, no_opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = "1,,3";
        const auto opt = vsl::ReSplitOptions::SKIP_EMPTY;
        const auto expected_res = {"1", "3"};
        auto res = vsl::re_split(s, re, opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = "1, ,3";
        const auto opt = vsl::ReSplitOptions::SKIP_EMPTY;
        const auto expected_res = {"1", " ", "3"};
        auto res = vsl::re_split(s, re, opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = "1, ,3";
        const auto opt = vsl::ReSplitOptions::TRIM | vsl::ReSplitOptions::SKIP_EMPTY;
        const auto expected_res = {"1", "3"};
        auto res = vsl::re_split(s, re, opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = "1";
        const auto expected_res = {"1"};
        auto res = vsl::re_split(s, re, no_opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = " 1 ";
        const auto opt = vsl::ReSplitOptions::TRIM;
        const auto expected_res = {"1"};
        auto res = vsl::re_split(s, re, opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = "  ";
        const auto opt = vsl::ReSplitOptions::SKIP_EMPTY;
        const auto expected_res = {"  "};
        auto res = vsl::re_split(s, re, opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
    {
        const auto s = "  ";
        const auto opt = vsl::ReSplitOptions::TRIM | vsl::ReSplitOptions::SKIP_EMPTY;
        const auto expected_res = empty_res;
        auto res = vsl::re_split(s, re, opt, no_flags);
        EXPECT_THAT(std::vector(res.begin(), res.end()), testing::ElementsAreArray(expected_res));
    }
}

TEST(RegexTest, Replace)
{
    auto test_replace = [](auto&& s, auto&& re, auto&& repl, vsl::ReReplFlags flags, auto&& expected_res)
    {
        auto res = std::string{};
        res.reserve(100);
        vsl::re_replace(res, s, re, repl, flags);
        EXPECT_EQ(res, expected_res);

        EXPECT_EQ(vsl::re_replace(s, re, repl, flags), expected_res);
    };

    const auto s = "Quick brown fox";
    const auto pattern = "a|e|i|o|u";
    const auto re = vsl::Re{pattern};
    const auto repl = "[$&]";
    const auto no_flags = vsl::ReReplFlags::DEFAULT;
    const auto expected_res = "Q[u][i]ck br[o]wn f[o]x";

    // arg: base
    test_replace(s, re, repl, no_flags, expected_res);

    // arg: s
    test_replace(std::string{s}, re, repl, no_flags, expected_res);
    test_replace(std::string_view{s}, re, repl, no_flags, expected_res);

    // arg: re
    test_replace(s, vsl::ReAscii{pattern}, repl, no_flags, expected_res);

    // arg: repl
    test_replace(s, re, std::string{repl}, no_flags, expected_res);
    test_replace(s, re, "[$&]", no_flags, expected_res);

    // arg: flags
    test_replace(s, re, repl, vsl::ReReplFlags::FORMAT_FIRST_ONLY, "Q[u]ick brown fox");
    test_replace(s, re, repl, vsl::ReReplFlags::FORMAT_NO_COPY, "[u][i][o][o]");

    auto flags =
        vsl::ReReplFlags::MATCH_CONTINUOUS | vsl::ReReplFlags::FORMAT_FIRST_ONLY | vsl::ReReplFlags::FORMAT_NO_COPY;
    test_replace(s, re, repl, flags, "");
    test_replace("example", re, repl, flags, "[e]");
}

TEST(RegexTest, ReplaceIter)
{
    auto test_replace = [](auto&& s, auto&& re, auto&& repl, vsl::ReReplFlags flags, auto&& expected_res)
    {
        auto res = std::string{"res = ["};
        res.reserve(100);
        auto it = std::back_inserter(res);
        it = vsl::re_replace(it, s.begin(), s.end(), re, repl, flags);
        ++it = ']';
        EXPECT_EQ(res, expected_res);
    };

    const auto s = std::string{"Quick brown fox"};
    const auto pattern = "a|e|i|o|u";
    const auto re = vsl::Re{pattern};
    const auto repl = "*";
    const auto no_flags = vsl::ReReplFlags::DEFAULT;
    const auto expected_res = "res = [Q**ck br*wn f*x]";

    // arg: base
    test_replace(s, re, repl, no_flags, expected_res);

    // arg: s
    test_replace(std::string_view{s}, re, repl, no_flags, expected_res);

    // arg: re
    test_replace(s, vsl::ReAscii{pattern}, repl, no_flags, expected_res);

    // arg: repl
    test_replace(s, re, std::string{repl}, no_flags, expected_res);
    test_replace(s, re, "*", no_flags, expected_res);

    // arg: flags
    test_replace(s, re, repl, vsl::ReReplFlags::FORMAT_FIRST_ONLY, "res = [Q*ick brown fox]");
    test_replace(s, re, repl, vsl::ReReplFlags::FORMAT_NO_COPY, "res = [****]");
    test_replace(s, re, repl, vsl::ReReplFlags::FORMAT_FIRST_ONLY | vsl::ReReplFlags::FORMAT_NO_COPY, "res = [*]");

    auto flags =
        vsl::ReReplFlags::MATCH_CONTINUOUS | vsl::ReReplFlags::FORMAT_FIRST_ONLY | vsl::ReReplFlags::FORMAT_NO_COPY;
    test_replace(s, re, repl, flags, "res = []");
    test_replace(std::string{"example"}, re, repl, flags, "res = [*]");
}

TEST(RegexTest, ReplaceFunc)
{
    auto test_replace_func = [](auto&& s, auto&& re, auto&& repl_func, vsl::ReReplFlags flags, auto&& expected_res)
    {
        auto res = std::string{};
        res.reserve(100);
        vsl::re_replace(res, s, re, repl_func, flags);
        EXPECT_EQ(res, expected_res);

        EXPECT_EQ(vsl::re_replace(s, re, repl_func, flags), expected_res);
    };

    auto repl_func = [](const vsl::ReMatch& match)
    {
        auto value = std::stoi(match.str());
        return std::to_string(value * 20);
    };

    const auto s = "A=1, B=2";
    const auto pattern = "\\d+";
    const auto re = vsl::Re{pattern};
    const auto no_flags = vsl::ReReplFlags::DEFAULT;
    const auto expected_res = "A=20, B=40";

    // arg: base
    test_replace_func(s, re, repl_func, no_flags, expected_res);

    // arg: s
    test_replace_func(std::string{s}, re, repl_func, no_flags, expected_res);
    test_replace_func(std::string_view{s}, re, repl_func, no_flags, expected_res);

    // arg: re
    test_replace_func(s, vsl::ReAscii{pattern}, repl_func, no_flags, expected_res);

    // arg: flags
    test_replace_func(s, re, repl_func, vsl::ReReplFlags::FORMAT_FIRST_ONLY, "A=20, B=2");
    test_replace_func(s, re, repl_func, vsl::ReReplFlags::FORMAT_NO_COPY, "2040");

    auto flags =
        vsl::ReReplFlags::MATCH_CONTINUOUS | vsl::ReReplFlags::FORMAT_FIRST_ONLY | vsl::ReReplFlags::FORMAT_NO_COPY;
    test_replace_func(s, re, repl_func, flags, "");
    test_replace_func("3 4 5", re, repl_func, flags, "60");

    test_replace_func("", re, repl_func, no_flags, "");
    test_replace_func("no match", re, repl_func, no_flags, "no match");
    test_replace_func("109", re, repl_func, no_flags, "2180");
    test_replace_func("1x3", re, repl_func, no_flags, "20x60");
}

TEST(RegexTest, MatchToStringView)
{
    const auto str = "123abc456";
    const auto re = vsl::Re{"\\d+(\\D+)\\d+"};
    auto match = vsl::ReMatch{};

    vsl::re_full_match(str, re, match);

    auto res1 = vsl::sv(match);
    auto res2 = vsl::sv(match[0]);
    auto res3 = vsl::sv(match[1]);

    EXPECT_EQ(res1, str);
    EXPECT_EQ(res2, str);
    EXPECT_EQ(res3, "abc");

    static_assert(std::is_same_v<decltype(res1), std::string_view>);
    static_assert(std::is_same_v<decltype(res2), std::string_view>);
    static_assert(std::is_same_v<decltype(res3), std::string_view>);
}

TEST(RegexTest, MatchFromBegin)
{
    // Usual search
    {
        const auto re = vsl::Re{"\\d+"};
        EXPECT_TRUE(vsl::re_search("abc123", re));
    }

    // Match from begin (using vsl::ReMatchFlags::MATCH_CONTINUOUS flag)
    {
        const auto re = vsl::Re{"\\d+"};
        EXPECT_FALSE(vsl::re_search("abc123", re, vsl::ReMatchFlags::MATCH_CONTINUOUS));
        EXPECT_TRUE(vsl::re_search("123abc", re, vsl::ReMatchFlags::MATCH_CONTINUOUS));
    }

    // Match from begin (using ^)
    {
        const auto re = vsl::Re{"^\\d+"};
        EXPECT_FALSE(vsl::re_search("abc123", re));
        EXPECT_TRUE(vsl::re_search("123abc", re));
    }
}

TEST(RegexTest, Utf8Support)
{
    const auto str = "Пример";
    const auto pattern = "и.е";

    const auto re_ascii = vsl::ReAscii(pattern);
    EXPECT_FALSE(vsl::re_search(str, re_ascii));

    const auto re = vsl::Re(pattern);
    auto match = vsl::ReMatch{};
    EXPECT_TRUE(vsl::re_search(str, re, match));
    EXPECT_EQ(match.str(), "име");
}

TEST(RegexTest, ReFlags)
{
    // multiline
    {
        const auto str = "123\nabc";
        const auto pattern = "^\\D+";
        EXPECT_FALSE(vsl::re_search(str, vsl::Re{pattern}));
        EXPECT_TRUE(vsl::re_search(str, vsl::Re{pattern, vsl::Re::multiline}));
    }

    // dotall
    {
        const auto str = "111\r\naaaX";
        const auto pattern = "^.+X";
        EXPECT_FALSE(vsl::re_search(str, vsl::Re{pattern}));
        EXPECT_TRUE(vsl::re_search(str, vsl::Re{pattern, vsl::Re::dotall}));
    }

    // icase (ascii)
    {
        const auto str = "EXAMPLE";
        const auto pattern = "a.p";
        auto match = vsl::ReMatch{};

        EXPECT_FALSE(vsl::re_search(str, vsl::ReAscii{pattern}));
        EXPECT_FALSE(vsl::re_search(str, vsl::Re{pattern}));

        EXPECT_TRUE(vsl::re_search(str, vsl::ReAscii{pattern, vsl::Re::icase}, match));
        EXPECT_EQ(match.str(), "AMP");

        EXPECT_TRUE(vsl::re_search(str, vsl::Re{pattern, vsl::Re::icase}, match));
        EXPECT_EQ(match.str(), "AMP");
    }

    // icase (utf-8)
    {
        const auto str = "ПРИМЕР";
        const auto pattern = "и.е";
        auto match = vsl::ReMatch{};

        EXPECT_FALSE(vsl::re_search(str, vsl::ReAscii{pattern}));
        EXPECT_FALSE(vsl::re_search(str, vsl::Re{pattern}));

        EXPECT_FALSE(vsl::re_search(str, vsl::ReAscii{pattern, vsl::Re::icase}));

        EXPECT_TRUE(vsl::re_search(str, vsl::Re{pattern, vsl::Re::icase}, match));
        EXPECT_EQ(match.str(), "ИМЕ");
    }
}

TEST(RegexTest, Escape)
{
    auto test_escape = [](auto&& s, auto&& expected_res)
    {
        auto res = std::string{};
        res.reserve(100);
        vsl::re_escape(res, s);
        EXPECT_EQ(res, expected_res);

        EXPECT_EQ(vsl::re_escape(s), expected_res);
    };

    test_escape("", "");
    test_escape("  ", "  ");
    test_escape("\n\t", "\n\t");
    test_escape(R"(.^$*+?|()[]{}\)", R"(\.\^\$\*\+\?\|\(\)\[\]\{\}\\)");
    test_escape(R"(!@#%&-_=~,;:/<>"'`)", R"(!@#%&-_=~,;:/<>"'`)");
    test_escape(R"(Цена: $100.00 (со скидкой 20%))", R"(Цена: \$100\.00 \(со скидкой 20%\))");
}

TEST(RegexTest, EscapeRepl)
{
    auto test_escape_repl = [](auto&& s, auto&& expected_res)
    {
        auto res = std::string{};
        res.reserve(100);
        vsl::re_escape_repl(res, s);
        EXPECT_EQ(res, expected_res);

        EXPECT_EQ(vsl::re_escape_repl(s), expected_res);
    };

    test_escape_repl("", "");
    test_escape_repl("  ", "  ");
    test_escape_repl("\n\t", "\n\t");
    test_escape_repl("$", "$$");
    test_escape_repl("$$", "$$$$");
    test_escape_repl(R"(.^*+?|()[]{}\!@#%&-_=~,;:/<>"'`)", R"(.^*+?|()[]{}\!@#%&-_=~,;:/<>"'`)");
    test_escape_repl(R"(Цена: $100.00 (со скидкой 20%))", R"(Цена: $$100.00 (со скидкой 20%))");
}

}  // namespace test
