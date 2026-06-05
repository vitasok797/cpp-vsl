#include "reflection.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fmt/format.h>

namespace test
{

TEST(ReflectionTest, Nameof)
{
    const auto var = 0;

    EXPECT_EQ(NAMEOF(var), "var");
    EXPECT_EQ(NAMEOF_TYPE(decltype(var)), "int");
}

TEST(ReflectionTest, NameofFmt)
{
    const auto var = 0;

    EXPECT_EQ(fmt::format("<{}>", NAMEOF(var)), "<var>");
    EXPECT_EQ(fmt::format("<{}>", NAMEOF_SHORT_TYPE(decltype(var))), "<int>");
}

}  // namespace test
