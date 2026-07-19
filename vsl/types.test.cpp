#include "types.h"

#include <vsl/concepts.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfloat>
#include <cmath>
#include <cstdint>
#include <limits>

namespace test
{

TEST(TypesTest, Constants)
{
    EXPECT_EQ(vsl::I8_MIN, INT8_MIN);
    EXPECT_EQ(vsl::I8_MAX, INT8_MAX);
    EXPECT_EQ(vsl::I16_MIN, INT16_MIN);
    EXPECT_EQ(vsl::I16_MAX, INT16_MAX);
    EXPECT_EQ(vsl::I32_MIN, INT32_MIN);
    EXPECT_EQ(vsl::I32_MAX, INT32_MAX);
    EXPECT_EQ(vsl::I64_MIN, INT64_MIN);
    EXPECT_EQ(vsl::I64_MAX, INT64_MAX);

    EXPECT_EQ(vsl::U8_MIN, 0);
    EXPECT_EQ(vsl::U8_MAX, UINT8_MAX);
    EXPECT_EQ(vsl::U16_MIN, 0);
    EXPECT_EQ(vsl::U16_MAX, UINT16_MAX);
    EXPECT_EQ(vsl::U32_MIN, 0);
    EXPECT_EQ(vsl::U32_MAX, UINT32_MAX);
    EXPECT_EQ(vsl::U64_MIN, 0);
    EXPECT_EQ(vsl::U64_MAX, UINT64_MAX);

    EXPECT_EQ(vsl::DOUBLE_MAX, DBL_MAX);
    EXPECT_EQ(vsl::DOUBLE_MAX_NEG, -DBL_MAX);
    EXPECT_EQ(vsl::INF, HUGE_VAL);
}

TEST(TypesTest, NarrowCast)
{
    EXPECT_EQ(vsl::narrow_cast<int>(double{1.1}), 1);
}

TEST(TypesTest, NumericCastBool)
{
    EXPECT_EQ(vsl::numeric_cast<bool>(i8{0}), false);
    EXPECT_EQ(vsl::numeric_cast<bool>(i8{1}), true);

    EXPECT_EQ(vsl::numeric_cast<bool>(float{0.0f}), false);
    EXPECT_EQ(vsl::numeric_cast<bool>(float{1.0f}), true);

    EXPECT_EQ(vsl::numeric_cast<bool>(double{0.0}), false);
    EXPECT_EQ(vsl::numeric_cast<bool>(double{1.0}), true);

    EXPECT_THROW(vsl::numeric_cast<bool>(i8{2}), vsl::NarrowingError);

    EXPECT_THROW(vsl::numeric_cast<bool>(float{1.1f}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<bool>(double{1.1}), vsl::NarrowingError);
}

TEST(TypesTest, NumericCastFloating)
{
    EXPECT_EQ(vsl::numeric_cast<float>(bool{false}), 0.0f);
    EXPECT_EQ(vsl::numeric_cast<float>(bool{true}), 1.0f);
    EXPECT_EQ(vsl::numeric_cast<double>(bool{false}), 0.0);
    EXPECT_EQ(vsl::numeric_cast<double>(bool{true}), 1.0);

    EXPECT_EQ(vsl::numeric_cast<float>(i32{7}), 7.0f);
    EXPECT_EQ(vsl::numeric_cast<double>(i32{7}), 7.0);

    EXPECT_EQ(vsl::numeric_cast<float>(u32{7}), 7.0f);
    EXPECT_EQ(vsl::numeric_cast<double>(u32{7}), 7.0);

    EXPECT_EQ(vsl::numeric_cast<float>(i32{-7}), -7.0f);
    EXPECT_EQ(vsl::numeric_cast<double>(i32{-7}), -7.0);

    EXPECT_THROW(vsl::numeric_cast<float>(vsl::U32_MAX), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<double>(vsl::U64_MAX), vsl::NarrowingError);

    EXPECT_THROW(vsl::numeric_cast<i32>(std::numeric_limits<double>::quiet_NaN()), vsl::NarrowingError);
}

TEST(TypesTest, NumericCastSigned)
{
    EXPECT_EQ(vsl::numeric_cast<i16>(bool{false}), 0);
    EXPECT_EQ(vsl::numeric_cast<i16>(bool{true}), 1);

    EXPECT_EQ(vsl::numeric_cast<i16>(i8{0}), 0);
    EXPECT_EQ(vsl::numeric_cast<i16>(i16{0}), 0);
    EXPECT_EQ(vsl::numeric_cast<i16>(i32{0}), 0);

    EXPECT_EQ(vsl::numeric_cast<i16>(u8{0}), 0);
    EXPECT_EQ(vsl::numeric_cast<i16>(u16{0}), 0);
    EXPECT_EQ(vsl::numeric_cast<i16>(u32{0}), 0);

    EXPECT_EQ(vsl::numeric_cast<i16>(i8{7}), 7);
    EXPECT_EQ(vsl::numeric_cast<i16>(i16{7}), 7);
    EXPECT_EQ(vsl::numeric_cast<i16>(i32{7}), 7);

    EXPECT_EQ(vsl::numeric_cast<i16>(i8{-7}), -7);
    EXPECT_EQ(vsl::numeric_cast<i16>(i16{-7}), -7);
    EXPECT_EQ(vsl::numeric_cast<i16>(i32{-7}), -7);

    EXPECT_EQ(vsl::numeric_cast<i16>(u8{7}), 7);
    EXPECT_EQ(vsl::numeric_cast<i16>(u16{7}), 7);
    EXPECT_EQ(vsl::numeric_cast<i16>(u32{7}), 7);

    EXPECT_EQ(vsl::numeric_cast<i16>(vsl::I16_MIN), vsl::I16_MIN);
    EXPECT_EQ(vsl::numeric_cast<i16>(vsl::I16_MAX), vsl::I16_MAX);
    EXPECT_THROW(vsl::numeric_cast<i16>(i32{vsl::I16_MIN - 1}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<i16>(i32{vsl::I16_MAX + 1}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<i16>(u16{vsl::I16_MAX + 1}), vsl::NarrowingError);

    EXPECT_EQ(vsl::numeric_cast<i16>(float{7.0f}), 7);
    EXPECT_EQ(vsl::numeric_cast<i16>(double{7.0}), 7);
    EXPECT_EQ(vsl::numeric_cast<i16>(float{-7.0f}), -7);
    EXPECT_EQ(vsl::numeric_cast<i16>(double{-7.0}), -7);
    EXPECT_EQ(vsl::numeric_cast<i16>(double{1e4}), 10000);
    EXPECT_EQ(vsl::numeric_cast<i16>(double{-1e4}), -10000);
    EXPECT_THROW(vsl::numeric_cast<i16>(float{1.1f}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<i16>(double{1.1}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<i16>(double{1e5}), vsl::NarrowingError);
}

TEST(TypesTest, NumericCastUnsigned)
{
    EXPECT_EQ(vsl::numeric_cast<u16>(bool{false}), 0);
    EXPECT_EQ(vsl::numeric_cast<u16>(bool{true}), 1);

    EXPECT_EQ(vsl::numeric_cast<u16>(i8{0}), 0);
    EXPECT_EQ(vsl::numeric_cast<u16>(i16{0}), 0);
    EXPECT_EQ(vsl::numeric_cast<u16>(i32{0}), 0);

    EXPECT_EQ(vsl::numeric_cast<u16>(u8{0}), 0);
    EXPECT_EQ(vsl::numeric_cast<u16>(u16{0}), 0);
    EXPECT_EQ(vsl::numeric_cast<u16>(u32{0}), 0);

    EXPECT_EQ(vsl::numeric_cast<u16>(i8{7}), 7);
    EXPECT_EQ(vsl::numeric_cast<u16>(i16{7}), 7);
    EXPECT_EQ(vsl::numeric_cast<u16>(i32{7}), 7);

    EXPECT_THROW(vsl::numeric_cast<u16>(i8{-1}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<u16>(i16{-1}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<u16>(i32{-1}), vsl::NarrowingError);

    EXPECT_EQ(vsl::numeric_cast<u16>(u8{7}), 7);
    EXPECT_EQ(vsl::numeric_cast<u16>(u16{7}), 7);
    EXPECT_EQ(vsl::numeric_cast<u16>(u32{7}), 7);

    EXPECT_EQ(vsl::numeric_cast<u16>(vsl::U16_MAX), vsl::U16_MAX);
    EXPECT_THROW(vsl::numeric_cast<u16>(i32{vsl::U16_MAX + 1}), vsl::NarrowingError);

    EXPECT_EQ(vsl::numeric_cast<u16>(float{7.0f}), 7);
    EXPECT_EQ(vsl::numeric_cast<u16>(double{7.0}), 7);
    EXPECT_EQ(vsl::numeric_cast<u16>(double{1e4}), 10000);
    EXPECT_THROW(vsl::numeric_cast<u16>(float{-1.0f}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<u16>(double{-1.0}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<u16>(float{1.1f}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<u16>(double{1.1}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<u16>(double{1e5}), vsl::NarrowingError);
}

TEST(TypesTest, AsSigned)
{
    {
        // uint8_t -> int16_t
        const auto value = uint8_t{200};
        const auto result = vsl::as_signed(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), int16_t>);
    }
    {
        // uint16_t -> int32_t
        const auto value = uint16_t{60000};
        const auto result = vsl::as_signed(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), int32_t>);
    }
    {
        // uint32_t -> int64_t
        const auto value = uint32_t{4'000'000'000};
        const auto result = vsl::as_signed(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), int64_t>);
    }
    {
        // uint64_t -> int64_t within range
        const auto value = uint64_t{1'000'000'000'000};
        const auto result = vsl::as_signed(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), int64_t>);
    }
    {
        // Handle zero
        EXPECT_EQ(vsl::as_signed(uint8_t{0}), int16_t{0});
        EXPECT_EQ(vsl::as_signed(uint16_t{0}), int32_t{0});
        EXPECT_EQ(vsl::as_signed(uint32_t{0}), int64_t{0});
        EXPECT_EQ(vsl::as_signed(uint64_t{0}), int64_t{0});
    }
    {
        // Handle max values
        EXPECT_EQ(vsl::as_signed(std::numeric_limits<uint8_t>::max()), int16_t{255});
        EXPECT_EQ(vsl::as_signed(std::numeric_limits<uint16_t>::max()), int32_t{65535});
        EXPECT_EQ(vsl::as_signed(std::numeric_limits<uint32_t>::max()), int64_t{4'294'967'295});
    }
    {
        // Throw on int64_t overflow
        const auto value = static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1;
        EXPECT_THROW({ [[maybe_unused]] auto result = vsl::as_signed(value); }, vsl::NarrowingError);
    }
}

TEST(TypesTest, AsSignedUnchecked)
{
    {
        // uint8_t -> int16_t
        const auto value = uint8_t{200};
        const auto result = vsl::as_signed_unchecked(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), int16_t>);
    }
    {
        // uint16_t -> int32_t
        const auto value = uint16_t{60000};
        const auto result = vsl::as_signed_unchecked(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), int32_t>);
    }
    {
        // uint32_t -> int64_t
        const auto value = uint32_t{4'000'000'000};
        const auto result = vsl::as_signed_unchecked(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), int64_t>);
    }
    {
        // uint64_t -> int64_t within range
        const auto value = uint64_t{1'000'000'000'000};
        const auto result = vsl::as_signed_unchecked(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), int64_t>);
    }
    {
        // Handle zero
        EXPECT_EQ(vsl::as_signed_unchecked(uint8_t{0}), int16_t{0});
        EXPECT_EQ(vsl::as_signed_unchecked(uint16_t{0}), int32_t{0});
        EXPECT_EQ(vsl::as_signed_unchecked(uint32_t{0}), int64_t{0});
        EXPECT_EQ(vsl::as_signed_unchecked(uint64_t{0}), int64_t{0});
    }
    {
        // Handle max values
        EXPECT_EQ(vsl::as_signed_unchecked(std::numeric_limits<uint8_t>::max()), int16_t{255});
        EXPECT_EQ(vsl::as_signed_unchecked(std::numeric_limits<uint16_t>::max()), int32_t{65535});
        EXPECT_EQ(vsl::as_signed_unchecked(std::numeric_limits<uint32_t>::max()), int64_t{4'294'967'295});
    }
    {
        // Check noexcept
        static_assert(noexcept(vsl::as_signed_unchecked(uint8_t{})));
        static_assert(noexcept(vsl::as_signed_unchecked(uint16_t{})));
        static_assert(noexcept(vsl::as_signed_unchecked(uint32_t{})));
        static_assert(noexcept(vsl::as_signed_unchecked(uint64_t{})));
    }
}

TEST(TypesTest, AsUnsigned)
{
    {
        // Positive int8_t -> uint8_t
        const auto value = int8_t{100};
        const auto result = vsl::as_unsigned(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), uint8_t>);
    }
    {
        // Positive int16_t -> uint16_t
        const auto value = int16_t{30000};
        const auto result = vsl::as_unsigned(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), uint16_t>);
    }
    {
        // Positive int32_t -> uint32_t
        const auto value = int32_t{2'000'000'000};
        const auto result = vsl::as_unsigned(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), uint32_t>);
    }
    {
        // Positive int64_t -> uint64_t
        const auto value = int64_t{9'000'000'000'000'000'000};
        const auto result = vsl::as_unsigned(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), uint64_t>);
    }
    {
        // Handle zero
        EXPECT_EQ(vsl::as_unsigned(int8_t{0}), uint8_t{0});
        EXPECT_EQ(vsl::as_unsigned(int16_t{0}), uint16_t{0});
        EXPECT_EQ(vsl::as_unsigned(int32_t{0}), uint32_t{0});
        EXPECT_EQ(vsl::as_unsigned(int64_t{0}), uint64_t{0});
    }
    {
        // Handle max positive values
        EXPECT_EQ(vsl::as_unsigned(std::numeric_limits<int8_t>::max()), uint8_t{127});
        EXPECT_EQ(vsl::as_unsigned(std::numeric_limits<int16_t>::max()), uint16_t{32767});
        EXPECT_EQ(vsl::as_unsigned(std::numeric_limits<int32_t>::max()), uint32_t{2'147'483'647});
        EXPECT_EQ(vsl::as_unsigned(std::numeric_limits<int64_t>::max()), uint64_t{9'223'372'036'854'775'807});
    }
    {
        // Throw on negative
        EXPECT_THROW({ [[maybe_unused]] auto result = vsl::as_unsigned(int8_t{-1}); }, vsl::NarrowingError);
        EXPECT_THROW({ [[maybe_unused]] auto result = vsl::as_unsigned(int16_t{-1}); }, vsl::NarrowingError);
        EXPECT_THROW({ [[maybe_unused]] auto result = vsl::as_unsigned(int32_t{-1}); }, vsl::NarrowingError);
        EXPECT_THROW({ [[maybe_unused]] auto result = vsl::as_unsigned(int64_t{-1}); }, vsl::NarrowingError);
    }
}

TEST(TypesTest, AsUnsignedUnchecked)
{
    {
        // Positive int8_t -> uint8_t
        const auto value = int8_t{100};
        const auto result = vsl::as_unsigned_unchecked(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), uint8_t>);
    }
    {
        // Positive int16_t -> uint16_t
        const auto value = int16_t{30000};
        const auto result = vsl::as_unsigned_unchecked(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), uint16_t>);
    }
    {
        // Positive int32_t -> uint32_t
        const auto value = int32_t{2'000'000'000};
        const auto result = vsl::as_unsigned_unchecked(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), uint32_t>);
    }
    {
        // Positive int64_t -> uint64_t
        const auto value = int64_t{9'000'000'000'000'000'000};
        const auto result = vsl::as_unsigned_unchecked(value);
        EXPECT_EQ(result, value);
        static_assert(vsl::same_type_as<decltype(result), uint64_t>);
    }
    {
        // Handle zero
        EXPECT_EQ(vsl::as_unsigned_unchecked(int8_t{0}), uint8_t{0});
        EXPECT_EQ(vsl::as_unsigned_unchecked(int16_t{0}), uint16_t{0});
        EXPECT_EQ(vsl::as_unsigned_unchecked(int32_t{0}), uint32_t{0});
        EXPECT_EQ(vsl::as_unsigned_unchecked(int64_t{0}), uint64_t{0});
    }
    {
        // Handle max positive values
        EXPECT_EQ(vsl::as_unsigned_unchecked(std::numeric_limits<int8_t>::max()), uint8_t{127});
        EXPECT_EQ(vsl::as_unsigned_unchecked(std::numeric_limits<int16_t>::max()), uint16_t{32767});
        EXPECT_EQ(vsl::as_unsigned_unchecked(std::numeric_limits<int32_t>::max()), uint32_t{2'147'483'647});
        EXPECT_EQ(vsl::as_unsigned_unchecked(std::numeric_limits<int64_t>::max()), uint64_t{9'223'372'036'854'775'807});
    }
    {
        // Check noexcept
        static_assert(noexcept(vsl::as_unsigned_unchecked(int8_t{})));
        static_assert(noexcept(vsl::as_unsigned_unchecked(int16_t{})));
        static_assert(noexcept(vsl::as_unsigned_unchecked(int32_t{})));
        static_assert(noexcept(vsl::as_unsigned_unchecked(int64_t{})));
    }
    {
        // Negative values
        EXPECT_EQ(vsl::as_unsigned_unchecked(int8_t{-1}), uint8_t{255});
        EXPECT_EQ(vsl::as_unsigned_unchecked(int32_t{-1}), uint32_t{4'294'967'295});
    }
}

}  // namespace test
