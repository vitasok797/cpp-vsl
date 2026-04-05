#include "text.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>

namespace vsl::test
{

TEST(TextTest, Indent)
{
    EXPECT_EQ(indent("AAA\nBBB\nCCC", 2), "  AAA\n  BBB\n  CCC");
    EXPECT_EQ(indent("AAA\nBBB\nCCC\n", 2), "  AAA\n  BBB\n  CCC\n");
    EXPECT_EQ(indent("AAA\r\nBBB\r\nCCC", 2), "  AAA\r\n  BBB\r\n  CCC");
    EXPECT_EQ(indent("AAA\nBBB\nCCC", 0), "AAA\nBBB\nCCC");
    EXPECT_EQ(indent("AAA\nBBB\nCCC", -1), "AAA\nBBB\nCCC");
    EXPECT_EQ(indent("\n\nAAA\nBBB\n\n\nCCC\n\n", 1), "\n\n AAA\n BBB\n\n\n CCC\n\n");
    EXPECT_EQ(indent("", 1), "");
    EXPECT_EQ(indent(" ", 1), "  ");
    EXPECT_EQ(indent(" \n ", 1), "  \n  ");
    EXPECT_EQ(indent("AAA", 2), "  AAA");
}

TEST(TextTest, OutOf)
{
    EXPECT_EQ(out_of(3, 3), "3");
    EXPECT_EQ(out_of(3, 4), "3/4");
    EXPECT_EQ(out_of(int16_t{3}, uint64_t{4}), "3/4");
}

}  // namespace vsl::test
