#include "result.h"

#include <vsl/debug.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <concepts>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

using namespace testing;

namespace test
{

namespace
{

struct ValueWithGreedyCtor
{
    ValueWithGreedyCtor() = default;

    template<typename T>
    ValueWithGreedyCtor(T&&)
    {}
};

struct ResultErrorSink
{
    explicit ResultErrorSink(vsl::ResultError<int>)
    {}
};

struct ThrowingMove
{
    ThrowingMove() = default;
    ThrowingMove(const ThrowingMove&) = default;

    ThrowingMove(ThrowingMove&&) noexcept(false)
    {}
};

}  // namespace

TEST(ResultTest, DefaultConstructionHasValue)
{
    const auto result = vsl::Result<int, std::string>{};

    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result);
    EXPECT_EQ(result.value(), 0);
}

TEST(ResultTest, ValueConstruction)
{
    const auto result = vsl::Result<int, std::string>{42};

    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result);
    EXPECT_EQ(result.value(), 42);
}

TEST(ResultTest, ErrorConstruction)
{
    const auto result = vsl::Result<int, std::string>{vsl::ResultError{std::string{"error"}}};

    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error(), "error");
}

TEST(ResultTest, ValueThrowsWhenContainsError)
{
    const auto result = vsl::Result<int, std::string>{vsl::ResultError{std::string{"error"}}};

    EXPECT_THROW(static_cast<void>(result.value()), std::bad_variant_access);
}

TEST(ResultTest, ErrorThrowsWhenContainsValue)
{
    const auto result = vsl::Result<int, std::string>{42};

    EXPECT_THROW(static_cast<void>(result.error()), std::bad_variant_access);
}

TEST(ResultTest, ValueOrReturnsValue)
{
    const auto result = vsl::Result<int, std::string>{42};

    EXPECT_EQ(result.value_or(100), 42);
}

TEST(ResultTest, ValueOrReturnsDefault)
{
    const auto result = vsl::Result<int, std::string>{vsl::ResultError{std::string{"error"}}};

    EXPECT_EQ(result.value_or(100), 100);
}

TEST(ResultTest, VoidDefaultConstructionHasValue)
{
    const auto result = vsl::Result<void, std::string>{};

    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result);
}

TEST(ResultTest, VoidErrorConstruction)
{
    const auto result = vsl::Result<void, std::string>{vsl::ResultError{std::string{"error"}}};

    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error(), "error");
}

TEST(ResultTest, VoidErrorThrowsWhenContainsValue)
{
    const auto result = vsl::Result<void, std::string>{};

    EXPECT_THROW(static_cast<void>(result.error()), std::bad_variant_access);
}

TEST(ResultTest, ValueLvalueAccessorCopies)
{
    auto stats = vsl::CopyMoveStats{};
    auto value = vsl::CopyMoveProbe{stats};

    auto result = vsl::Result<vsl::CopyMoveProbe, int>{value};

    EXPECT_TRUE(stats.only_copies(1));

    stats.clear();

    [[maybe_unused]] auto extracted = result.value();

    EXPECT_TRUE(stats.only_copies(1));
}

TEST(ResultTest, ValueRvalueAccessorMoves)
{
    auto stats = vsl::CopyMoveStats{};
    auto value = vsl::CopyMoveProbe{stats};

    auto result = vsl::Result<vsl::CopyMoveProbe, int>{std::move(value)};

    EXPECT_TRUE(stats.only_moves(1));

    stats.clear();

    [[maybe_unused]] auto extracted = std::move(result).value();

    EXPECT_TRUE(stats.only_moves(1));
}

TEST(ResultTest, ErrorLvalueAccessorCopies)
{
    auto stats = vsl::CopyMoveStats{};
    auto error = vsl::CopyMoveProbe{stats};

    auto result_error = vsl::ResultError{error};
    auto result = vsl::Result<int, vsl::CopyMoveProbe>{result_error};

    EXPECT_TRUE(stats.only_copies(2));

    stats.clear();

    [[maybe_unused]] auto extracted = result.error();

    EXPECT_TRUE(stats.only_copies(1));
}

TEST(ResultTest, ErrorRvalueAccessorMoves)
{
    auto stats = vsl::CopyMoveStats{};
    auto error = vsl::CopyMoveProbe{stats};

    auto result = vsl::Result<int, vsl::CopyMoveProbe>{
        vsl::ResultError{std::move(error)},
    };

    EXPECT_TRUE(stats.only_moves(2));

    stats.clear();

    [[maybe_unused]] auto extracted = std::move(result).error();

    EXPECT_TRUE(stats.only_moves(1));
}

TEST(ResultTest, ValueOrLvalueCopiesContainedValue)
{
    auto value_stats = vsl::CopyMoveStats{};
    auto default_stats = vsl::CopyMoveStats{};

    auto value = vsl::CopyMoveProbe{value_stats};
    auto default_value = vsl::CopyMoveProbe{default_stats};

    auto result = vsl::Result<vsl::CopyMoveProbe, int>{std::move(value)};

    value_stats.clear();
    default_stats.clear();

    [[maybe_unused]] auto extracted = result.value_or(default_value);

    EXPECT_TRUE(value_stats.only_copies(1));
    EXPECT_TRUE(default_stats.no_ops());
}

TEST(ResultTest, ValueOrRvalueMovesContainedValue)
{
    auto value_stats = vsl::CopyMoveStats{};
    auto default_stats = vsl::CopyMoveStats{};

    auto value = vsl::CopyMoveProbe{value_stats};
    auto default_value = vsl::CopyMoveProbe{default_stats};

    auto result = vsl::Result<vsl::CopyMoveProbe, int>{std::move(value)};

    value_stats.clear();
    default_stats.clear();

    [[maybe_unused]] auto extracted = std::move(result).value_or(std::move(default_value));

    EXPECT_TRUE(value_stats.only_moves(1));
    EXPECT_TRUE(default_stats.no_ops());
}

TEST(ResultTest, ValueOrLvalueCopiesDefaultValue)
{
    auto stats = vsl::CopyMoveStats{};
    auto default_value = vsl::CopyMoveProbe{stats};

    auto result = vsl::Result<vsl::CopyMoveProbe, int>{vsl::ResultError{42}};

    [[maybe_unused]] auto extracted = result.value_or(default_value);

    EXPECT_TRUE(stats.only_copies(1));
}

TEST(ResultTest, ValueOrRvalueMovesDefaultValue)
{
    auto stats = vsl::CopyMoveStats{};
    auto default_value = vsl::CopyMoveProbe{stats};

    auto result = vsl::Result<vsl::CopyMoveProbe, int>{vsl::ResultError{42}};

    [[maybe_unused]] auto extracted = std::move(result).value_or(std::move(default_value));

    EXPECT_TRUE(stats.only_moves(1));
}

TEST(ResultTest, ResultCopyCopiesValue)
{
    auto stats = vsl::CopyMoveStats{};
    auto value = vsl::CopyMoveProbe{stats};

    const auto source = vsl::Result<vsl::CopyMoveProbe, int>{std::move(value)};

    stats.clear();

    [[maybe_unused]] auto copy = source;

    EXPECT_TRUE(stats.only_copies(1));
}

TEST(ResultTest, ResultMoveMovesValue)
{
    auto stats = vsl::CopyMoveStats{};
    auto value = vsl::CopyMoveProbe{stats};

    auto source = vsl::Result<vsl::CopyMoveProbe, int>{std::move(value)};

    stats.clear();

    [[maybe_unused]] auto moved = std::move(source);

    EXPECT_TRUE(stats.only_moves(1));
}

TEST(ResultTest, ResultErrorIsNotHijackedByValueConstructor)
{
    using Result = vsl::Result<ValueWithGreedyCtor, int>;

    auto source = Result{vsl::ResultError{42}};

    const auto copy = Result{source};

    ASSERT_FALSE(copy.has_value());
    EXPECT_EQ(copy.error(), 42);
}

TEST(ResultTest, ResultErrorCannotFallThroughToValueConstructor)
{
    using Result = vsl::Result<ResultErrorSink, std::string>;

    static_assert(!std::constructible_from<Result, vsl::ResultError<int>>);
}

// Compile-time interface checks

static_assert(std::same_as<decltype(std::declval<const vsl::Result<int, std::string>&>().value()), const int&>);
static_assert(std::same_as<decltype(std::declval<vsl::Result<int, std::string>&&>().value()), int&&>);
static_assert(std::same_as<decltype(std::declval<const vsl::Result<int, std::string>&>().error()), const std::string&>);
static_assert(std::same_as<decltype(std::declval<vsl::Result<int, std::string>&&>().error()), std::string&&>);
static_assert(
    std::same_as<decltype(std::declval<const vsl::Result<void, std::string>&>().error()), const std::string&>);
static_assert(std::same_as<decltype(std::declval<vsl::Result<void, std::string>&&>().error()), std::string&&>);

// Result is immutable, but can be copied/moved into a new object

using RegularResult = vsl::Result<int, std::string>;

static_assert(std::is_copy_constructible_v<RegularResult>);
static_assert(std::is_move_constructible_v<RegularResult>);

static_assert(!std::is_copy_assignable_v<RegularResult>);
static_assert(!std::is_move_assignable_v<RegularResult>);

using VoidResult = vsl::Result<void, std::string>;

static_assert(std::is_copy_constructible_v<VoidResult>);
static_assert(std::is_move_constructible_v<VoidResult>);

static_assert(!std::is_copy_assignable_v<VoidResult>);
static_assert(!std::is_move_assignable_v<VoidResult>);

// Move constructor must inherit noexcept from V/E rather than claiming it unconditionally

static_assert(!std::is_nothrow_move_constructible_v<vsl::Result<ThrowingMove, int>>);

static_assert(!std::is_nothrow_move_constructible_v<vsl::Result<void, ThrowingMove>>);

}  // namespace test
