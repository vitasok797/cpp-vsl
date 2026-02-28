#include "pointer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

namespace vsl::test
{

static auto good_ptr = std::make_shared<int>(1);
static auto bad_ptr = std::shared_ptr<int>{nullptr};

TEST(PointerTest, CheckPtr)
{
    check_ptr(good_ptr);
    EXPECT_THROW(check_ptr(bad_ptr), NullPointerError);
}

TEST(PointerTest, CheckedDerefPtr)
{
    auto& ref = checked_deref_ptr(good_ptr);
    EXPECT_EQ(&ref, good_ptr.get());
    EXPECT_EQ(ref, 1);

    EXPECT_THROW(checked_deref_ptr(bad_ptr), NullPointerError);
}

TEST(PointerTest, CheckedGetPtr)
{
    auto* ptr = checked_get_ptr(good_ptr);
    EXPECT_EQ(ptr, good_ptr.get());

    EXPECT_THROW(checked_get_ptr(bad_ptr), NullPointerError);
}

TEST(PointerTest, AsPtr)
{
    auto opt = std::optional<int>{1};
    EXPECT_EQ(*as_ptr(opt), 1);

    const auto opt_const = std::optional<int>{2};
    EXPECT_EQ(*as_ptr(opt_const), 2);

    auto opt_null = std::optional<int>{std::nullopt};
    EXPECT_EQ(as_ptr(opt_null), nullptr);

    const auto opt_const_null = std::optional<int>{std::nullopt};
    EXPECT_EQ(as_ptr(opt_const_null), nullptr);
}

}  // namespace vsl::test
