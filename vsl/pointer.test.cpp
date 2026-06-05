#include "pointer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <optional>

namespace test
{

static const auto good_ptr = std::make_shared<int>(1);
static const auto bad_ptr = std::shared_ptr<int>{nullptr};

TEST(PointerTest, CheckPtr)
{
    vsl::check_ptr(good_ptr);
    EXPECT_THROW(vsl::check_ptr(bad_ptr), vsl::NullPointerError);
}

TEST(PointerTest, CheckedDerefPtr)
{
    auto& ref = vsl::checked_deref_ptr(good_ptr);
    EXPECT_EQ(&ref, good_ptr.get());
    EXPECT_EQ(ref, 1);

    EXPECT_THROW(vsl::checked_deref_ptr(bad_ptr), vsl::NullPointerError);
}

TEST(PointerTest, CheckedGetPtr)
{
    auto* ptr = vsl::checked_get_ptr(good_ptr);
    EXPECT_EQ(ptr, good_ptr.get());

    EXPECT_THROW(vsl::checked_get_ptr(bad_ptr), vsl::NullPointerError);
}

TEST(PointerTest, AsPtr)
{
    auto opt = std::optional<int>{1};
    EXPECT_EQ(*vsl::as_ptr(opt), 1);

    const auto opt_const = std::optional<int>{2};
    EXPECT_EQ(*vsl::as_ptr(opt_const), 2);

    auto opt_null = std::optional<int>{std::nullopt};
    EXPECT_EQ(vsl::as_ptr(opt_null), nullptr);

    const auto opt_const_null = std::optional<int>{std::nullopt};
    EXPECT_EQ(vsl::as_ptr(opt_const_null), nullptr);
}

}  // namespace test
