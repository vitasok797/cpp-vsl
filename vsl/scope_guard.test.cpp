#include "scope_guard.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>
#include <vector>

using namespace testing;

namespace test
{

TEST(ScopeGuardTest, OnScopeExit)
{
    {
        auto called = false;

        {
            const auto guard = vsl::ScopeGuard{[&] { called = true; }};
            EXPECT_FALSE(called);
        }

        EXPECT_TRUE(called);
    }
    {
        auto called = false;

        {
            auto lam = [&] { called = true; };
            const auto guard = vsl::ScopeGuard{lam};
            EXPECT_FALSE(called);
        }

        EXPECT_TRUE(called);
    }
}

TEST(ScopeGuardTest, Dismiss)
{
    auto called = false;

    {
        auto guard = vsl::ScopeGuard{[&] { called = true; }};
        guard.dismiss();
    }

    EXPECT_FALSE(called);
}

TEST(ScopeGuardTest, MultipleGuards)
{
    auto calls = std::vector<int>{};

    {
        const auto guard1 = vsl::ScopeGuard{[&] { calls.push_back(1); }};
        const auto guard2 = vsl::ScopeGuard{[&] { calls.push_back(2); }};
        const auto guard3 = vsl::ScopeGuard{[&] { calls.push_back(3); }};
    }

    EXPECT_EQ(calls, (std::vector{3, 2, 1}));
}

TEST(ScopeGuardTest, OnException)
{
    auto called = false;

    EXPECT_THROW(
        {
            const auto guard = vsl::ScopeGuard{[&] { called = true; }};
            throw std::runtime_error{"error"};
        },
        std::runtime_error);

    EXPECT_TRUE(called);
}

TEST(ScopeGuardTest, Macro)
{
    auto called = false;

    {
        VSL_SCOPE_GUARD
        {
            called = true;
        };
        EXPECT_FALSE(called);
    }

    EXPECT_TRUE(called);
}

}  // namespace test
