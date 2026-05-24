#include "util.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <vector>

namespace vsl::test
{

TEST(UtilTest, IsOneOf)
{
    enum class Options
    {
        OPT1,
        OPT2,
        OPT3,
        OPT4,
        OPT5,
    };

    auto opt = Options::OPT3;

    EXPECT_TRUE(vsl::is_one_of(opt, Options::OPT1, Options::OPT3, Options::OPT4));
    EXPECT_FALSE(vsl::is_one_of(opt, Options::OPT1, Options::OPT2, Options::OPT4));

    EXPECT_TRUE(vsl::is_one_of(opt, {Options::OPT1, Options::OPT3, Options::OPT4}));
    EXPECT_FALSE(vsl::is_one_of(opt, {Options::OPT1, Options::OPT2, Options::OPT4}));

    EXPECT_TRUE(vsl::is_one_of(opt, std::vector{Options::OPT1, Options::OPT3, Options::OPT4}));
    EXPECT_FALSE(vsl::is_one_of(opt, std::vector{Options::OPT1, Options::OPT2, Options::OPT4}));

    EXPECT_TRUE(vsl::is_one_of(opt, std::array{Options::OPT1, Options::OPT3, Options::OPT4}));
    EXPECT_FALSE(vsl::is_one_of(opt, std::array{Options::OPT1, Options::OPT2, Options::OPT4}));
}

template<typename Underlying>
struct GetSizeMixin
{
    auto get_size() const -> int
    {
        const Underlying& und = vsl::this_to<Underlying>(this);
        return und.size;
    }
};

struct ClassWithMixin : GetSizeMixin<ClassWithMixin>
{
    int size{7};
};

TEST(UtilTest, ThisTo)
{
    EXPECT_EQ(ClassWithMixin{}.get_size(), 7);
}

}  // namespace vsl::test
