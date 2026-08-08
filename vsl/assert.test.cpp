#include "assert.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

namespace test
{

TEST(AssertTest, Expect)
{
    auto x = 1;

    ASSERT_NO_THROW(VSL_EXPECT(x == 1));
    ASSERT_NO_THROW(VSL_EXPECT(x == 1, "desc"));

    EXPECT_THAT([&] { VSL_EXPECT(x == 2); },
                ThrowsMessage<vsl::AssertionError>(
                    AllOf(HasSubstr("Assertion failed"), HasSubstr("Expect: x == 2"), HasSubstr("Description: n/a"))));

    EXPECT_THAT([&] { VSL_EXPECT(x == 3, "desc"); },
                ThrowsMessage<vsl::AssertionError>(
                    AllOf(HasSubstr("Assertion failed"), HasSubstr("Expect: x == 3"), HasSubstr("Description: desc"))));
}

TEST(AssertTest, Assert)
{
    auto x = 1;

    ASSERT_NO_THROW(VSL_ASSERT(x == 1));
    ASSERT_NO_THROW(VSL_ASSERT(x == 1, "desc"));

    EXPECT_THAT([&] { VSL_ASSERT(x == 2); },
                ThrowsMessage<vsl::AssertionError>(
                    AllOf(HasSubstr("Assertion failed"), HasSubstr("Assert: x == 2"), HasSubstr("Description: n/a"))));

    EXPECT_THAT([&] { VSL_ASSERT(x == 3, "desc"); },
                ThrowsMessage<vsl::AssertionError>(
                    AllOf(HasSubstr("Assertion failed"), HasSubstr("Assert: x == 3"), HasSubstr("Description: desc"))));
}

TEST(AssertTest, Ensure)
{
    auto x = 1;

    ASSERT_NO_THROW(VSL_ENSURE(x == 1));
    ASSERT_NO_THROW(VSL_ENSURE(x == 1, "desc"));

    EXPECT_THAT([&] { VSL_ENSURE(x == 2); },
                ThrowsMessage<vsl::AssertionError>(
                    AllOf(HasSubstr("Assertion failed"), HasSubstr("Ensure: x == 2"), HasSubstr("Description: n/a"))));

    EXPECT_THAT([&] { VSL_ENSURE(x == 3, "desc"); },
                ThrowsMessage<vsl::AssertionError>(
                    AllOf(HasSubstr("Assertion failed"), HasSubstr("Ensure: x == 3"), HasSubstr("Description: desc"))));
}

}  // namespace test
