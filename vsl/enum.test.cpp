#include "enum.h"

#include <vsl/types.h>
#include <vsl/util.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <string>
#include <string_view>

namespace vsl::test
{

enum class Color
{
    RED = 100,
    GREEN = 101,
    BLUE = 102,
};

TEST(EnumTest, EnumContainsEnum)
{
    EXPECT_TRUE(vsl::enum_contains<Color>(static_cast<Color>(102)));
    EXPECT_FALSE(vsl::enum_contains<Color>(static_cast<Color>(999)));
}

TEST(EnumTest, EnumContainsUnderlying)
{
    EXPECT_TRUE(vsl::enum_contains<Color>(102));
    EXPECT_FALSE(vsl::enum_contains<Color>(999));
}

TEST(EnumTest, EnumContainsString)
{
    EXPECT_TRUE(vsl::enum_contains<Color>("RED"));
    EXPECT_FALSE(vsl::enum_contains<Color>("red"));
    EXPECT_FALSE(vsl::enum_contains<Color>("UNKNOWN"));
    EXPECT_FALSE(vsl::enum_contains<Color>("unknown"));
}

TEST(EnumTest, EnumContainsStringIcase)
{
    EXPECT_TRUE(vsl::enum_contains_icase<Color>("RED"));
    EXPECT_TRUE(vsl::enum_contains_icase<Color>("red"));
    EXPECT_FALSE(vsl::enum_contains_icase<Color>("UNKNOWN"));
    EXPECT_FALSE(vsl::enum_contains_icase<Color>("unknown"));
}

TEST(EnumTest, EnumCount)
{
    EXPECT_EQ(vsl::enum_count<Color>(), 3);
}

TEST(EnumTest, EnumEntries)
{
    constexpr auto color_entries = vsl::enum_entries<Color>();

    EXPECT_EQ(color_entries[0].first, Color::RED);
    EXPECT_EQ(color_entries[0].second, "RED");

    EXPECT_EQ(color_entries[1].first, Color::GREEN);
    EXPECT_EQ(color_entries[1].second, "GREEN");

    EXPECT_EQ(color_entries[2].first, Color::BLUE);
    EXPECT_EQ(color_entries[2].second, "BLUE");
}

TEST(EnumTest, EnumFromString)
{
    EXPECT_EQ(vsl::enum_from_string<Color>("BLUE").value(), Color::BLUE);
    EXPECT_FALSE(vsl::enum_from_string<Color>("blue").has_value());
    EXPECT_FALSE(vsl::enum_from_string<Color>("UNKNOWN").has_value());
    EXPECT_FALSE(vsl::enum_from_string<Color>("unknown").has_value());
}

TEST(EnumTest, EnumFromStringIcase)
{
    EXPECT_EQ(vsl::enum_from_string_icase<Color>("BLUE").value(), Color::BLUE);
    EXPECT_EQ(vsl::enum_from_string_icase<Color>("blue").value(), Color::BLUE);
    EXPECT_FALSE(vsl::enum_from_string_icase<Color>("UNKNOWN").has_value());
    EXPECT_FALSE(vsl::enum_from_string_icase<Color>("unknown").has_value());
}

TEST(EnumTest, EnumFromUnderlying)
{
    EXPECT_EQ(vsl::enum_cast<Color>(102).value(), Color::BLUE);
    EXPECT_FALSE(vsl::enum_cast<Color>(999).has_value());
}

TEST(EnumTest, EnumIndex)
{
    EXPECT_EQ(vsl::enum_index<Color::GREEN>(), 1);
    EXPECT_EQ(vsl::enum_index(Color::BLUE).value(), 2);
    EXPECT_FALSE(vsl::enum_index(static_cast<Color>(999)).has_value());
}

TEST(EnumTest, EnumInteger)
{
    EXPECT_EQ(vsl::enum_integer(Color::GREEN), 101);
}

TEST(EnumTest, EnumName)
{
    EXPECT_EQ(vsl::enum_name(Color::RED), "RED");
}

TEST(EnumTest, EnumNames)
{
    EXPECT_EQ(vsl::enum_names<Color>(), (std::array<std::string_view, 3>{"RED", "GREEN", "BLUE"}));
}

TEST(EnumTest, EnumSwitch)
{
    // Example:
    // https://github.com/Neargye/magic_enum/blob/master/example/example_switch.cpp

    auto switcher = vsl::overloaded{[](vsl::enum_constant<Color::GREEN>) { return "spec: GREEN"; },
                                    [](Color other) { return "default: " + std::string{vsl::enum_name(other)}; }};

    EXPECT_EQ(vsl::enum_switch<std::string>(switcher, Color::GREEN), "spec: GREEN");
    EXPECT_EQ(vsl::enum_switch<std::string>(switcher, Color::RED), "default: RED");
    EXPECT_EQ(vsl::enum_switch<std::string>(switcher, static_cast<Color>(-3)), "");
}

TEST(EnumTest, EnumTypeName)
{
    EXPECT_EQ(vsl::enum_type_name<Color>(), "Color");
}

TEST(EnumTest, EnumValues)
{
    EXPECT_EQ(vsl::enum_values<Color>(), (std::array{Color::RED, Color::GREEN, Color::BLUE}));
}

enum class Options : u32
{
    NONE = 0,
    A = u32{1} << 0,
    B = u32{1} << 1,
    C = u32{1} << 2,
};
VSL_DECLARE_ENUM_FLAGS(Options)

static constexpr auto NONE = Options::NONE;
static constexpr auto A = Options::A;
static constexpr auto B = Options::B;
static constexpr auto C = Options::C;

TEST(EnumTest, BitwiseOperators)
{
    auto flags = A;
    flags |= B;
    EXPECT_EQ(flags, A | B);

    EXPECT_EQ(flags & A, A);
    EXPECT_EQ(flags & C, NONE);

    EXPECT_EQ((~A) & A, NONE);
}

TEST(EnumTest, EnumContainsFlags)
{
    EXPECT_EQ(vsl::enum_contains_flags(A, B), false);
    EXPECT_EQ(vsl::enum_contains_flags(A, A), true);

    EXPECT_EQ(vsl::enum_contains_flags(A | C, B), false);
    EXPECT_EQ(vsl::enum_contains_flags(A | C, C), true);

    EXPECT_EQ(vsl::enum_contains_flags(A, A | C), false);
    EXPECT_EQ(vsl::enum_contains_flags(A | B, A | C), false);
    EXPECT_EQ(vsl::enum_contains_flags(A | C, A | C), true);
    EXPECT_EQ(vsl::enum_contains_flags(A | B | C, A | C), true);

    EXPECT_EQ(vsl::enum_contains_flags(NONE, NONE), false);
    EXPECT_EQ(vsl::enum_contains_flags(NONE, A), false);

    EXPECT_EQ(vsl::enum_contains_flags(A, NONE), false);
    EXPECT_EQ(vsl::enum_contains_flags(B | C, NONE), false);
}

}  // namespace vsl::test
