#include "container.h"

#include <vsl/types.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace testing;

namespace test
{

TEST(ContainerTest, AsRvalueView)
{
    auto make_vector_from_range_1 = [](auto&& r)
    {
        using T = std::ranges::range_value_t<decltype(r)>;
        return std::vector<T>(std::ranges::begin(r), std::ranges::end(r));
    };

    auto make_vector_from_range_2 = [](auto&& r)
    {
        using T = std::ranges::range_value_t<decltype(r)>;
        auto out = std::vector<T>{};
        std::ranges::copy(r, std::back_inserter(out));
        return out;
    };

    {
        // Copy items
        auto src = std::vector<std::string>{"a", "b", "c"};
        auto res = make_vector_from_range_1(src);
        EXPECT_EQ(res, (std::vector<std::string>{"a", "b", "c"}));
        EXPECT_EQ(src, (std::vector<std::string>{"a", "b", "c"}));
    }
    {
        // Copy items
        auto src = std::vector<std::string>{"a", "b", "c"};
        auto res = make_vector_from_range_2(src);
        EXPECT_EQ(res, (std::vector<std::string>{"a", "b", "c"}));
        EXPECT_EQ(src, (std::vector<std::string>{"a", "b", "c"}));
    }
    {
        // Copy items (vsl::view_as_rvalue + const vector)
        const auto src = std::vector<std::string>{"a", "b", "c"};
        auto res = make_vector_from_range_1(src | vsl::view_as_rvalue);
        EXPECT_EQ(res, (std::vector<std::string>{"a", "b", "c"}));
        EXPECT_EQ(src, (std::vector<std::string>{"a", "b", "c"}));
    }
    {
        // Copy items (vsl::view_as_rvalue + const vector)
        const auto src = std::vector<std::string>{"a", "b", "c"};
        auto res = make_vector_from_range_2(src | vsl::view_as_rvalue);
        EXPECT_EQ(res, (std::vector<std::string>{"a", "b", "c"}));
        EXPECT_EQ(src, (std::vector<std::string>{"a", "b", "c"}));
    }
    {
        // Move itmes (vsl::view_as_rvalue + mutable vector)
        auto src = std::vector<std::string>{"a", "b", "c"};
        auto res = make_vector_from_range_1(src | vsl::view_as_rvalue);
        EXPECT_EQ(res, (std::vector<std::string>{"a", "b", "c"}));
        EXPECT_EQ(src, (std::vector<std::string>{"", "", ""}));  // Items are moved, so vector may be cleared
        src.clear();
    }
    {
        // Move itmes (vsl::view_as_rvalue + mutable vector)
        auto src = std::vector<std::string>{"a", "b", "c"};
        auto res = make_vector_from_range_2(src | vsl::view_as_rvalue);
        EXPECT_EQ(res, (std::vector<std::string>{"a", "b", "c"}));
        EXPECT_EQ(src, (std::vector<std::string>{"", "", ""}));  // Items are moved, so vector may be cleared
        src.clear();
    }
    {
        // Move items (range with prvalue items)
        auto prvalue_range = std::views::iota(0, 3) | std::views::transform([](int i) { return std::to_string(i); });
        auto res = std::vector<std::string>{};
        std::ranges::copy(prvalue_range | vsl::view_as_rvalue, std::back_inserter(res));
        EXPECT_EQ(res, (std::vector<std::string>{"0", "1", "2"}));
    }
}

TEST(ContainerTest, StringHash)
{
    const auto hasher = vsl::StringHash{};

    const auto cstr = "test_value";
    const auto str = std::string{"test_value"};
    const auto sv = std::string_view{"test_value"};

    EXPECT_EQ(hasher(cstr), hasher(sv));
    EXPECT_EQ(hasher(cstr), hasher(str));
    EXPECT_NE(hasher("apple"), hasher("banana"));
}

TEST(ContainerTest, StringHashUnorderedSet)
{
    auto set = std::unordered_set<std::string, vsl::StringHash, std::equal_to<>>{};

    set.insert("hello_world");

    const auto it1 = set.find("hello_world");
    EXPECT_NE(it1, set.end());
    EXPECT_EQ(*it1, "hello_world");

    const auto it2 = set.find(std::string_view{"hello_world"});
    EXPECT_NE(it2, set.end());
    EXPECT_EQ(*it2, "hello_world");

    const auto it3 = set.find(std::string{"hello_world"});
    EXPECT_NE(it3, set.end());
    EXPECT_EQ(*it3, "hello_world");

    EXPECT_EQ(set.find("not_found"), set.end());
}

TEST(ContainerTest, StringHashUnorderedMap)
{
    auto map = std::unordered_map<std::string, int, vsl::StringHash, std::equal_to<>>{};

    map["config_value"] = 42;

    EXPECT_EQ(map.count("config_value"), 1);
    EXPECT_EQ(map.count(std::string_view{"config_value"}), 1);
    EXPECT_EQ(map.count(std::string{"config_value"}), 1);

    const auto it1 = map.find("config_value");
    ASSERT_NE(it1, map.end());
    EXPECT_EQ(it1->second, 42);

    const auto it2 = map.find(std::string_view{"config_value"});
    ASSERT_NE(it2, map.end());
    EXPECT_EQ(it2->second, 42);

    const auto it3 = map.find(std::string{"config_value"});
    ASSERT_NE(it3, map.end());
    EXPECT_EQ(it3->second, 42);

    EXPECT_EQ(map.find("not_found"), map.end());
}

TEST(ContainerTest, StringAsciiCaselessCompare)
{
    const auto comp = vsl::StringAsciiCaselessCompare{};

    EXPECT_FALSE(comp("HELLO", "hello"));
    EXPECT_FALSE(comp("hello", "HELLO"));
    EXPECT_FALSE(comp("", ""));

    EXPECT_TRUE(comp("abc", "abd"));
    EXPECT_TRUE(comp("ABC", "abd"));
    EXPECT_TRUE(comp("a", "b"));
    EXPECT_FALSE(comp("b", "a"));

    EXPECT_TRUE(comp("abc", "abcd"));
    EXPECT_FALSE(comp("abcd", "abc"));
}

TEST(ContainerTest, StringAsciiCaselessCompareSet)
{
    auto caseless_set = std::set<std::string, vsl::StringAsciiCaselessCompare>{};

    const auto [it1, inserted1] = caseless_set.insert("HelloWorld");
    EXPECT_TRUE(inserted1);
    EXPECT_EQ(caseless_set.size(), 1);

    const auto [it2, inserted2] = caseless_set.insert("helloworld");
    EXPECT_FALSE(inserted2);
    EXPECT_EQ(caseless_set.size(), 1);

    const auto it_find_cstr = caseless_set.find("HELLOWORLD");
    ASSERT_NE(it_find_cstr, caseless_set.end());
    EXPECT_EQ(*it_find_cstr, "HelloWorld");

    const auto it_find_sv = caseless_set.find(std::string_view{"helloWorld"});
    EXPECT_NE(it_find_sv, caseless_set.end());
    EXPECT_EQ(*it_find_sv, "HelloWorld");
}

TEST(ContainerTest, StringAsciiCaselessCompareMap)
{
    auto caseless_map = std::map<std::string, int, vsl::StringAsciiCaselessCompare>{};

    caseless_map["MaxConnections"] = 100;

    const auto it = caseless_map.find("maxconnections");
    ASSERT_NE(it, caseless_map.end());
    EXPECT_EQ(it->second, 100);

    EXPECT_EQ(caseless_map.count(std::string_view{"MAXCONNECTIONS"}), 1);

    caseless_map[std::string{"maxconnections"}] = 200;
    EXPECT_EQ(caseless_map.size(), 1);
    EXPECT_EQ(caseless_map["MaxConnections"], 200);
}

TEST(ContainerTest, GetHashTableFillEfficiencyEmptyContainer)
{
    auto empty_set = std::unordered_set<int>{};
    EXPECT_DOUBLE_EQ(vsl::get_hash_table_fill_efficiency(empty_set), 1.0);

    auto empty_map = std::unordered_map<std::string, int>{};
    EXPECT_DOUBLE_EQ(vsl::get_hash_table_fill_efficiency(empty_map), 1.0);
}

struct PerfectHash
{
    auto operator()(int key) const noexcept -> size_t
    {
        return static_cast<size_t>(key);
    }
};

TEST(ContainerTest, GetHashTableFillEfficiencyNoCollisions)
{
    auto set = std::unordered_set<int, PerfectHash>{};
    set.rehash(10);  // Ensure there are more buckets than elements to avoid automatic rehashing

    for (auto i = 0; i < 5; ++i)
    {
        set.insert(i);
    }

    // 5 elements occupied 5 different buckets. 5 / 5 = 1.0
    EXPECT_DOUBLE_EQ(vsl::get_hash_table_fill_efficiency(set), 1.0);
}

struct AlwaysZeroHash
{
    auto operator()(int) const noexcept -> size_t
    {
        return 0;
    }
};

TEST(ContainerTest, GetHashTableFillEfficiencyExtremeCollisions)
{
    auto set = std::unordered_set<int, AlwaysZeroHash>{};
    set.rehash(10);  // Ensure there are more buckets than elements to avoid automatic rehashing

    for (auto i = 0; i < 5; ++i)
    {
        set.insert(i);
    }

    // All 5 elements landed in bucket #0 because the hash is always 0.
    // occupied_buckets = 1, size = 5.
    // Expected efficiency: 1.0 / 5.0 = 0.2
    EXPECT_DOUBLE_EQ(vsl::get_hash_table_fill_efficiency(set), 0.2);

    // Additional check: verify that everything is indeed in a single bucket
    EXPECT_EQ(set.bucket_size(0), 5);
    for (auto i = size_t{1}; i < set.bucket_count(); ++i)
    {
        EXPECT_EQ(set.bucket_size(i), 0);
    }
}

}  // namespace test
