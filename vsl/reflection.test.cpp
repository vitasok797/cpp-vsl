#include "reflection.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace vsl::test
{

TEST(ReflectionTest, NameOf)
{
    const auto var = 0;

    EXPECT_EQ(NAMEOF(var), "var");
    EXPECT_EQ(NAMEOF_TYPE(decltype(var)), "int");
}

}  // namespace vsl::test
