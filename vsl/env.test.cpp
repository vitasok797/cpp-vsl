#include "env.h"

#include <vsl/os.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

namespace test
{

#ifdef VSL_WINDOWS_OS
static constexpr auto VAR_NAME = "ComSpec";
static constexpr auto VAR_SUBSTRING = "cmd.exe";
#else
static constexpr auto VAR_NAME = "PATH";
static constexpr auto VAR_SUBSTRING = "/home/";
#endif

TEST(EnvTest, Exists)
{
    EXPECT_TRUE(vsl::env_exists(VAR_NAME));
    EXPECT_FALSE(vsl::env_exists("UNKNOWN"));
}

TEST(EnvTest, Get)
{
    using namespace testing;

    {
        const auto res = vsl::env_get<std::string>(VAR_NAME);
        EXPECT_THAT(res, HasSubstr(VAR_SUBSTRING));
    }
    {
        auto call = [] { vsl::env_get<int>(VAR_NAME); };
        EXPECT_THAT(call,
                    ThrowsMessage<vsl::EnvParseError>(MatchesRegex("Parser error for environment variable '\\w+'.*")));
    }
    {
        auto call = [] { vsl::env_get<int>("UNKNOWN"); };
        EXPECT_THAT(call, ThrowsMessage<vsl::EnvNotFound>(StartsWith("Environment variable 'UNKNOWN' not set")));
    }
#ifdef VSL_WINDOWS_OS
    {
        const auto res = vsl::env_get<int>("NUMBER_OF_PROCESSORS");
        EXPECT_TRUE(res > 0);
    }
#endif
}

TEST(EnvTest, GetOr)
{
    {
        const auto res = vsl::env_get_or<std::string>(VAR_NAME, "default");
        EXPECT_NE(res, "default");
    }
    {
        const auto res = vsl::env_get_or<std::string>("UNKNOWN", "default");
        EXPECT_EQ(res, "default");
    }
    {
        const auto res = vsl::env_get_or<int>(VAR_NAME, 0);
        EXPECT_EQ(res, 0);
    }
}

}  // namespace test
