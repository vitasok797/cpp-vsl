#include "flat_map.h"

#include <vsl/container.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace testing;

namespace test
{

static const auto KeyNotFoundErrorMatcher = ThrowsMessage<std::out_of_range>(HasSubstr("key not found"));
static const auto DuplicateKeyErrorMatcher = ThrowsMessage<std::invalid_argument>(HasSubstr("duplicate key"));

TEST(FlatMapTest, EmptyTable)
{
    const auto table = vsl::FlatMap<int, int>{};

    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);

    EXPECT_FALSE(table.contains(42));

    EXPECT_EQ(table.find(42), nullptr);

    EXPECT_THAT([&] { [[maybe_unused]] auto val = table.at(42); }, KeyNotFoundErrorMatcher);
}

TEST(FlatMapTest, SingleElement)
{
    const auto table = vsl::FlatMap<char, int>{
        {'b', 0}
    };

    EXPECT_FALSE(table.empty());
    EXPECT_EQ(table.size(), 1);

    EXPECT_TRUE(table.contains('b'));
    EXPECT_FALSE(table.contains('a'));
    EXPECT_FALSE(table.contains('c'));

    ASSERT_NE(table.find('b'), nullptr);
    EXPECT_EQ(*table.find('b'), 0);
    EXPECT_EQ(table.find('a'), nullptr);
    EXPECT_EQ(table.find('c'), nullptr);

    EXPECT_EQ(table.at('b'), 0);
    EXPECT_THAT([&] { [[maybe_unused]] auto val = table.at('a'); }, KeyNotFoundErrorMatcher);
    EXPECT_THAT([&] { [[maybe_unused]] auto val = table.at('c'); }, KeyNotFoundErrorMatcher);
}

TEST(FlatMapTest, MultipleUnsortedElements)
{
    const auto table = vsl::FlatMap<std::string, int>{
        {  "dog", 1},
        {  "cat", 2},
        {"mouse", 3},
    };

    EXPECT_FALSE(table.empty());
    EXPECT_EQ(table.size(), 3);

    EXPECT_TRUE(table.contains("dog"));
    EXPECT_FALSE(table.contains("unknown"));

    ASSERT_NE(table.find("cat"), nullptr);
    EXPECT_EQ(*table.find("cat"), 2);
    EXPECT_EQ(table.find("unknown"), nullptr);

    EXPECT_EQ(table.at("mouse"), 3);
    EXPECT_THAT([&] { [[maybe_unused]] auto val = table.at("unknown"); }, KeyNotFoundErrorMatcher);
}

TEST(FlatMapTest, DuplicateKeyError)
{
    auto create_table_1 = []
    {
        const auto table = vsl::FlatMap<int, int>{
            {1, 100},
            {2, 200},
            {3, 300},
            {2, 400}, // Duplicate key
            {5, 500},
        };
    };

    auto create_table_2 = []
    {
        const auto table = vsl::FlatMap<int, int>{
            {1, 100},
            {1, 200}, // Duplicate key
            {3, 300},
            {4, 400},
            {5, 500},
        };
    };

    auto create_table_3 = []
    {
        const auto table = vsl::FlatMap<int, int>{
            {1, 100},
            {2, 200},
            {3, 300},
            {4, 400},
            {4, 500}, // Duplicate key
        };
    };

    EXPECT_THAT(create_table_1, DuplicateKeyErrorMatcher);
    EXPECT_THAT(create_table_2, DuplicateKeyErrorMatcher);
    EXPECT_THAT(create_table_3, DuplicateKeyErrorMatcher);
}

TEST(FlatMapTest, StringHeterogeneousLookup)
{
    const auto table = vsl::FlatMap<std::string, int>{
        {"aaa", 1},
        {"bbb", 2},
        {"ccc", 3},
    };

    EXPECT_TRUE(table.contains(std::string_view{"aaa"}));
    EXPECT_FALSE(table.contains(std::string_view{"unknown"}));

    EXPECT_EQ(*table.find(std::string_view{"bbb"}), 2);
    EXPECT_EQ(table.find(std::string_view{"unknown"}), nullptr);

    EXPECT_EQ(table.at(std::string_view{"ccc"}), 3);
    EXPECT_THAT([&] { [[maybe_unused]] auto val = table.at(std::string_view{"unknown"}); }, KeyNotFoundErrorMatcher);
}

TEST(FlatMapTest, KeySearch)
{
    const auto table = vsl::FlatMap<int, int>{
        {1, 11},
        {2, 22},
        {4, 44},
        {5, 55},
    };

    EXPECT_TRUE(table.contains(1));
    EXPECT_TRUE(table.contains(2));
    EXPECT_TRUE(table.contains(4));
    EXPECT_TRUE(table.contains(5));

    EXPECT_FALSE(table.contains(0));
    EXPECT_FALSE(table.contains(3));
    EXPECT_FALSE(table.contains(6));
}

enum class TestEnum
{
    OPT1,
    OPT2,
    OPT3,
};

TEST(FlatMapTest, Enum)
{
    const auto table = vsl::FlatMap<TestEnum, int>{
        {TestEnum::OPT1, 101},
        {TestEnum::OPT2, 102},
        {TestEnum::OPT3, 103},
    };

    EXPECT_EQ(table.at(TestEnum::OPT1), 101);
    EXPECT_EQ(table.at(TestEnum::OPT2), 102);
    EXPECT_EQ(table.at(TestEnum::OPT3), 103);
}

TEST(FlatMapTest, ComparatorGreater)
{
    const auto table = vsl::FlatMap<int, int, std::ranges::greater>{
        {1, 100},
        {5, 500},
        {2, 200},
        {4, 400},
    };

    EXPECT_FALSE(table.empty());
    EXPECT_EQ(table.size(), 4);

    EXPECT_TRUE(table.contains(1));
    EXPECT_TRUE(table.contains(2));
    EXPECT_TRUE(table.contains(4));
    EXPECT_TRUE(table.contains(5));

    EXPECT_EQ(*table.find(1), 100);
    EXPECT_EQ(*table.find(2), 200);
    EXPECT_EQ(table.at(4), 400);
    EXPECT_EQ(table.at(5), 500);

    EXPECT_FALSE(table.contains(0));
    EXPECT_FALSE(table.contains(3));
    EXPECT_FALSE(table.contains(6));
    EXPECT_EQ(table.find(10), nullptr);

    EXPECT_THAT([&] { [[maybe_unused]] auto val = table.at(99); }, KeyNotFoundErrorMatcher);
}

TEST(FlatMapTest, ComparatorLambda)
{
    auto comp = [](int a, int b) { return a > b; };

    const auto table = vsl::FlatMap<int, int, decltype(comp)>{
        {
         {5, 50},
         {2, 20},
         {1, 10},
         {4, 40},
         },
        comp
    };

    EXPECT_EQ(table.at(1), 10);
    EXPECT_EQ(table.at(2), 20);
    EXPECT_EQ(table.at(4), 40);
    EXPECT_EQ(table.at(5), 50);

    EXPECT_FALSE(table.contains(0));
    EXPECT_FALSE(table.contains(3));
    EXPECT_FALSE(table.contains(6));
}

TEST(FlatMapTest, ComparatorStringIcase)
{
    static constexpr auto ASCII_SYMBOLS_UPPER =
        R"( !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`{|}~)";
    static constexpr auto ASCII_SYMBOLS_LOWER =
        R"( !"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\]^_`{|}~)";

    const auto table = vsl::FlatMapAsciiIcase<std::string, int>{
        {            "Apple", 1},
        {           "Banana", 2},
        {ASCII_SYMBOLS_UPPER, 3},
    };

    EXPECT_TRUE(table.contains("apple"));  // Lower case
    EXPECT_EQ(*table.find("aPPLe"), 1);    // Mixed case
    EXPECT_EQ(table.at("BANANA"), 2);      // Upper case

    EXPECT_TRUE(table.contains(ASCII_SYMBOLS_LOWER));

    EXPECT_FALSE(table.contains("unknown"));
}

TEST(FlatMapTest, ComparatorStringIcaseDuplicateError)
{
    auto create_table = []
    {
        const auto table = vsl::FlatMap<std::string, int, vsl::StringAsciiIcaseCompare>{
            {"Hello", 1},
            {"HELLO", 2}, // Duplicate key
        };
    };

    EXPECT_THAT(create_table, DuplicateKeyErrorMatcher);
}

}  // namespace test
