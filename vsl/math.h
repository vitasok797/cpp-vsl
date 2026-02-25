#ifndef VSL_MATH_H
#define VSL_MATH_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <concepts>
#include <limits>
#include <numbers>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>

#define INF HUGE_VAL

namespace vsl
{

inline constexpr auto double_max = std::numeric_limits<double>::max();
inline constexpr auto double_max_neg = std::numeric_limits<double>::lowest();

inline auto almost_equal(double a, double b, double abs_epsilon) -> bool
{
    return std::abs(a - b) <= abs_epsilon;
}

inline auto almost_equal_rel(double a, double b, double rel_epsilon = 1e-6) -> bool
{
    return std::abs(a - b) <= (std::fmax(std::abs(a), std::abs(b)) * rel_epsilon);
}

inline auto ftrim(double value, std::pair<double, double> bounds) -> double
{
    auto [lo, hi] = bounds;
    if (lo > hi)
    {
        throw std::invalid_argument{"ftrim bounds error (low > high)"};
    }
    return std::clamp(value, lo, hi);
}

template<typename A, typename B>
    requires std::signed_integral<A> && std::signed_integral<B>
auto ceil_div(A a, B b) noexcept -> std::common_type_t<A, B>
{
    // TODO: use EXPECTS from vsl/assert.h
    assert(a >= 0);
    assert(b > 0);

    return a / b + (a % b ? 1 : 0);
}

// Integral: distribution [a, b], default [MAX_NEG, MAX_POS] and [a, MAX_POS]
// Floating point: distribution [a, b), default [0, 1) and [a, 1)
template<typename T>
    requires std::is_arithmetic_v<T> && (!std::is_same_v<T, bool>)
auto generate_random(T a = std::is_floating_point_v<T> ? T{0} : std::numeric_limits<T>::min(),
                     T b = std::is_floating_point_v<T> ? T{1} : std::numeric_limits<T>::max()) -> T
{
    static_assert(sizeof(T) > 1, "8-bit types (char, int8_t, uint8_t) are not supported by uniform_int_distribution");

    if (a > b)
    {
        throw std::invalid_argument{"interval bounds error (a > b)"};
    }

    thread_local static auto gen = std::mt19937_64{std::random_device{}()};

    if constexpr (std::is_floating_point_v<T>)
    {
        auto dis = std::uniform_real_distribution<T>{a, b};
        return dis(gen);
    }
    else
    {
        auto dis = std::uniform_int_distribution<T>{a, b};
        return dis(gen);
    }
}

}  // namespace vsl

#endif  // VSL_MATH_H
