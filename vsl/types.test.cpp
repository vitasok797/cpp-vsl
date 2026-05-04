#include "types.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfloat>
#include <cmath>
#include <cstdint>
#include <limits>

namespace vsl::test
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

    EXPECT_THROW(vsl::numeric_cast<float>(U32_MAX), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<double>(U64_MAX), vsl::NarrowingError);

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

    EXPECT_EQ(vsl::numeric_cast<i16>(I16_MIN), I16_MIN);
    EXPECT_EQ(vsl::numeric_cast<i16>(I16_MAX), I16_MAX);
    EXPECT_THROW(vsl::numeric_cast<i16>(i32{I16_MIN - 1}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<i16>(i32{I16_MAX + 1}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<i16>(u16{I16_MAX + 1}), vsl::NarrowingError);

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

    EXPECT_EQ(vsl::numeric_cast<u16>(U16_MAX), U16_MAX);
    EXPECT_THROW(vsl::numeric_cast<u16>(i32{U16_MAX + 1}), vsl::NarrowingError);

    EXPECT_EQ(vsl::numeric_cast<u16>(float{7.0f}), 7);
    EXPECT_EQ(vsl::numeric_cast<u16>(double{7.0}), 7);
    EXPECT_EQ(vsl::numeric_cast<u16>(double{1e4}), 10000);
    EXPECT_THROW(vsl::numeric_cast<u16>(float{-1.0f}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<u16>(double{-1.0}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<u16>(float{1.1f}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<u16>(double{1.1}), vsl::NarrowingError);
    EXPECT_THROW(vsl::numeric_cast<u16>(double{1e5}), vsl::NarrowingError);
}

}  // namespace vsl::test
