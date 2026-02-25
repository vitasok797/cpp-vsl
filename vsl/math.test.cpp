#include "math.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <climits>
#include <stdexcept>
#include <utility>

namespace vsl::test
{

TEST(MathTest, DoubleMax)
{
    EXPECT_EQ(double_max, DBL_MAX);
    EXPECT_EQ(double_max_neg, -DBL_MAX);
}

TEST(MathTest, AlmostEqual)
{
    EXPECT_TRUE(almost_equal(3.0, 4.0, 1.0));
    EXPECT_TRUE(almost_equal(3.0, 4.0, 1.1));
    EXPECT_FALSE(almost_equal(3.0, 4.0, 0.9));

    EXPECT_TRUE(almost_equal(-3.0, -4.0, 1.0));
    EXPECT_TRUE(almost_equal(-3.0, -4.0, 1.1));
    EXPECT_FALSE(almost_equal(-3.0, -4.0, 0.9));
}

TEST(MathTest, AlmostEqualRel)
{
    EXPECT_TRUE(almost_equal_rel(2.0, 2.000001));
    EXPECT_FALSE(almost_equal_rel(2.0, 2.00001));

    EXPECT_TRUE(almost_equal_rel(2.0, 2.001, 0.001));
    EXPECT_FALSE(almost_equal_rel(2.0, 2.01, 0.001));

    EXPECT_TRUE(almost_equal_rel(-2.0, -2.001, 0.001));
    EXPECT_FALSE(almost_equal_rel(-2.0, -2.01, 0.001));
}

TEST(MathTest, Ftrim)
{
    EXPECT_EQ(ftrim(-1.0, {0, 2.0}), 0);
    EXPECT_EQ(ftrim(1.0, {0, 2.0}), 1.0);
    EXPECT_EQ(ftrim(3.0, {0, 2.0}), 2.0);

    EXPECT_EQ(ftrim(100.0, {0, INF}), 100.0);
    EXPECT_EQ(ftrim(-100.0, {0, INF}), 0.0);

    EXPECT_EQ(ftrim(100.0, {-INF, 0}), 0.0);
    EXPECT_EQ(ftrim(-100.0, {-INF, 0}), -100.0);

    EXPECT_EQ(ftrim(5.0, {1.0, 1.0}), 1.0);

    EXPECT_THROW(ftrim(5.0, {1.0, -1.0}), std::invalid_argument);
}

TEST(MathTest, CeilDiv)
{
    EXPECT_EQ(ceil_div(0, 2), 0);

    EXPECT_EQ(ceil_div(4, 2), 2);
    EXPECT_EQ(ceil_div(5, 2), 3);
    EXPECT_EQ(ceil_div(6, 2), 3);

    EXPECT_EQ(ceil_div(int64_t{7}, 2), 4);
    EXPECT_EQ(ceil_div(int16_t{9}, 2), 5);
}

TEST(MathDeathTest, CeilDivAssertions)
{
    EXPECT_DEBUG_DEATH(ceil_div(-1, 2), "a >= 0");
    EXPECT_DEBUG_DEATH(ceil_div(1, 0), "b > 0");
}

template<typename T, typename Predicate, typename... Args>
auto test_generate_random(Predicate predicate, Args&&... args) -> void
{
    constexpr auto TEST_CYCLES = 10000;
    for (auto i = 0; i < TEST_CYCLES; ++i)
    {
        auto value = generate_random<T>(std::forward<Args>(args)...);
        ASSERT_TRUE(predicate(value)) << "Generated value: " << value;
    }
}

TEST(MathTest, GenerateRandomIntegral)
{
    auto full_default = [](auto x)
    {
        return x >= std::numeric_limits<decltype(x)>::min() && x <= std::numeric_limits<decltype(x)>::max();  //
    };
    ASSERT_NO_FATAL_FAILURE(test_generate_random<int64_t>(full_default));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<int32_t>(full_default));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<uint64_t>(full_default));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<uint32_t>(full_default));

    auto from0 = [](auto x) { return x >= 0 && x <= std::numeric_limits<decltype(x)>::max(); };
    ASSERT_NO_FATAL_FAILURE(test_generate_random<int64_t>(from0, 0));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<int32_t>(from0, 0));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<uint64_t>(from0, 0));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<uint32_t>(from0, 0));

    auto from1_to2 = [](auto x) { return x >= 1 && x <= 2; };
    ASSERT_NO_FATAL_FAILURE(test_generate_random<int64_t>(from1_to2, 1, 2));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<int32_t>(from1_to2, 1, 2));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<uint64_t>(from1_to2, 1, 2));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<uint32_t>(from1_to2, 1, 2));

    auto fromN100_toN10 = [](auto x) { return x >= -100 && x <= -10; };
    ASSERT_NO_FATAL_FAILURE(test_generate_random<int64_t>(fromN100_toN10, -100, -10));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<int32_t>(fromN100_toN10, -100, -10));

    ASSERT_THROW(generate_random<int64_t>(1, 0), std::invalid_argument);
}

TEST(MathTest, GenerateRandomFloating)
{
    auto full_default = [](auto x) { return x >= 0.0 && x < 1.0; };
    ASSERT_NO_FATAL_FAILURE(test_generate_random<float>(full_default));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<double>(full_default));

    auto from01 = [](auto x) { return x >= 0.1 && x < 1.0; };
    ASSERT_NO_FATAL_FAILURE(test_generate_random<float>(from01, 0.1f));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<double>(from01, 0.1));

    auto from1_to2 = [](auto x) { return x >= 1.0 && x < 2.0; };
    ASSERT_NO_FATAL_FAILURE(test_generate_random<float>(from1_to2, 1.0f, 2.0f));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<double>(from1_to2, 1.0, 2.0));

    auto fromN100_toN10 = [](auto x) { return x >= -100.0 && x < -10.0; };
    ASSERT_NO_FATAL_FAILURE(test_generate_random<float>(fromN100_toN10, -100.0f, -10.0f));
    ASSERT_NO_FATAL_FAILURE(test_generate_random<double>(fromN100_toN10, -100.0, -10.0));

    ASSERT_THROW(generate_random<double>(1.0, 0.0), std::invalid_argument);
}

}  // namespace vsl::test
