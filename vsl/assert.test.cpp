#include "assert.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

namespace test
{

TEST(AssertTest, Expects)
{
    auto x = 1;

    ASSERT_NO_THROW(VSL_EXPECTS(x == 1));
    ASSERT_NO_THROW(VSL_EXPECTS(x == 1, "desc"));

    EXPECT_THAT([&] { VSL_EXPECTS(x == 2); },
                ThrowsMessage<vsl::AssertionError>(
                    AllOf(HasSubstr("Assertion failed"), HasSubstr("Expects: x == 2"), HasSubstr("Description: n/a"))));

    EXPECT_THAT([&] { VSL_EXPECTS(x == 3, "desc"); },
                ThrowsMessage<vsl::AssertionError>(AllOf(HasSubstr("Assertion failed"), HasSubstr("Expects: x == 3"),
                                                         HasSubstr("Description: desc"))));
}

TEST(AssertTest, Ensures)
{
    auto x = 1;

    ASSERT_NO_THROW(VSL_ENSURES(x == 1));
    ASSERT_NO_THROW(VSL_ENSURES(x == 1, "desc"));

    EXPECT_THAT([&] { VSL_ENSURES(x == 2); },
                ThrowsMessage<vsl::AssertionError>(
                    AllOf(HasSubstr("Assertion failed"), HasSubstr("Ensures: x == 2"), HasSubstr("Description: n/a"))));

    EXPECT_THAT([&] { VSL_ENSURES(x == 3, "desc"); },
                ThrowsMessage<vsl::AssertionError>(AllOf(HasSubstr("Assertion failed"), HasSubstr("Ensures: x == 3"),
                                                         HasSubstr("Description: desc"))));
}

}  // namespace test
