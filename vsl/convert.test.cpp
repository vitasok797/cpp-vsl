#include "convert.h"

#include <vsl/types.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

using namespace testing;

namespace test
{

namespace
{

template<typename T>
concept NumericToStrCallable = requires(T value) { vsl::numeric_to_str(value); };

template<typename T>
concept StrToNumericCallable = requires { vsl::str_to_numeric<T>("0"); };

constexpr auto BOOL_0_1_ONLY = vsl::StrToNumericOptions{
    .bool_integral_0_1 = true,
    .bool_integral_0_ne0 = false,
    .bool_alpha = false,
};

constexpr auto BOOL_0_NE0_ONLY = vsl::StrToNumericOptions{
    .bool_integral_0_1 = false,
    .bool_integral_0_ne0 = true,
    .bool_alpha = false,
};

constexpr auto BOOL_ALPHA_ONLY = vsl::StrToNumericOptions{
    .bool_integral_0_1 = false,
    .bool_integral_0_ne0 = false,
    .bool_alpha = true,
};

constexpr auto BOOL_NONE = vsl::StrToNumericOptions{
    .bool_integral_0_1 = false,
    .bool_integral_0_ne0 = false,
    .bool_alpha = false,
};

}  // namespace

static_assert(NumericToStrCallable<bool>);
static_assert(NumericToStrCallable<int>);
static_assert(NumericToStrCallable<unsigned int>);
static_assert(NumericToStrCallable<float>);
static_assert(NumericToStrCallable<double>);

static_assert(!NumericToStrCallable<char>);
static_assert(!NumericToStrCallable<wchar_t>);
static_assert(!NumericToStrCallable<char8_t>);
static_assert(!NumericToStrCallable<char16_t>);
static_assert(!NumericToStrCallable<char32_t>);

static_assert(StrToNumericCallable<bool>);
static_assert(StrToNumericCallable<int>);
static_assert(StrToNumericCallable<unsigned int>);
static_assert(StrToNumericCallable<float>);
static_assert(StrToNumericCallable<double>);

static_assert(!StrToNumericCallable<char>);
static_assert(!StrToNumericCallable<wchar_t>);
static_assert(!StrToNumericCallable<char8_t>);
static_assert(!StrToNumericCallable<char16_t>);
static_assert(!StrToNumericCallable<char32_t>);

TEST(ConvertTest, NumericToStr)
{
    EXPECT_EQ(vsl::numeric_to_str(false), "false");
    EXPECT_EQ(vsl::numeric_to_str(true), "true");
    EXPECT_EQ(vsl::numeric_to_str(static_cast<bool>(123)), "true");

    EXPECT_EQ(vsl::numeric_to_str(0), "0");
    EXPECT_EQ(vsl::numeric_to_str(123), "123");
    EXPECT_EQ(vsl::numeric_to_str(-123), "-123");

    EXPECT_EQ(vsl::numeric_to_str(vsl::I64_MIN), "-9223372036854775808");
    EXPECT_EQ(vsl::numeric_to_str(vsl::I64_MAX), "9223372036854775807");
    EXPECT_EQ(vsl::numeric_to_str(vsl::U64_MAX), "18446744073709551615");

    EXPECT_EQ(vsl::numeric_to_str(0.0), "0");
    EXPECT_EQ(vsl::numeric_to_str(1.0), "1");
    EXPECT_EQ(vsl::numeric_to_str(1.1), "1.1");
    EXPECT_EQ(vsl::numeric_to_str(-123.45e50), "-1.2345e+52");
}

TEST(ConvertTest, NumericToStrAppendToOutput)
{
    auto res = std::string{"prefix:"};

    vsl::numeric_to_str(res, false);
    vsl::numeric_to_str(res, 123);
    vsl::numeric_to_str(res, 7.8);

    EXPECT_EQ(res, "prefix:false1237.8");
}

TEST(ConvertTest, NumericToStrFormattedBool)
{
    const auto format = vsl::NumericToStrFormat{
        .boolean = "[{:d}]",
    };

    EXPECT_EQ(vsl::numeric_to_str(false, format), "[0]");
    EXPECT_EQ(vsl::numeric_to_str(true, format), "[1]");
    EXPECT_EQ(vsl::numeric_to_str(static_cast<bool>(123), format), "[1]");
}

TEST(ConvertTest, NumericToStrFormattedIntegral)
{
    const auto format = vsl::NumericToStrFormat{
        .integral = "[{:+d}]",
    };

    EXPECT_EQ(vsl::numeric_to_str(0, format), "[+0]");
    EXPECT_EQ(vsl::numeric_to_str(123, format), "[+123]");
    EXPECT_EQ(vsl::numeric_to_str(-123, format), "[-123]");
}

TEST(ConvertTest, NumericToStrFormattedFloating)
{
    const auto format = vsl::NumericToStrFormat{
        .floating = "[{:E}]",
    };

    EXPECT_EQ(vsl::numeric_to_str(1.0, format), "[1.000000E+00]");
    EXPECT_EQ(vsl::numeric_to_str(1.1, format), "[1.100000E+00]");
    EXPECT_EQ(vsl::numeric_to_str(-123.45e50, format), "[-1.234500E+52]");
}

TEST(ConvertTest, NumericToStrFormattedAppendToOutput)
{
    const auto format = vsl::NumericToStrFormat{
        .boolean = "<{:d}>",
        .integral = "<{:+d}>",
        .floating = "<{:g}>",
    };

    auto res = std::string{"prefix:"};

    vsl::numeric_to_str(res, true, format);
    vsl::numeric_to_str(res, 123, format);
    vsl::numeric_to_str(res, 7.8, format);

    EXPECT_EQ(res, "prefix:<1><+123><7.8>");
}

TEST(ConvertTest, StrToNumericSignedIntegral)
{
    EXPECT_EQ(vsl::str_to_numeric<int>("0"), 0);
    EXPECT_EQ(vsl::str_to_numeric<int>("123"), 123);
    EXPECT_EQ(vsl::str_to_numeric<int>("-123"), -123);

    EXPECT_EQ(vsl::str_to_numeric<i64>("-9223372036854775808"), vsl::I64_MIN);
    EXPECT_EQ(vsl::str_to_numeric<i64>("9223372036854775807"), vsl::I64_MAX);
}

TEST(ConvertTest, StrToNumericUnsignedIntegral)
{
    EXPECT_EQ(vsl::str_to_numeric<unsigned int>("0"), 0u);
    EXPECT_EQ(vsl::str_to_numeric<unsigned int>("123"), 123u);

    EXPECT_EQ(vsl::str_to_numeric<u64>("18446744073709551615"), vsl::U64_MAX);
}

TEST(ConvertTest, StrToNumericFloating)
{
    EXPECT_EQ(vsl::str_to_numeric<double>("0"), 0.0);
    EXPECT_EQ(vsl::str_to_numeric<double>("1"), 1.0);
    EXPECT_EQ(vsl::str_to_numeric<double>("-1"), -1.0);

    EXPECT_EQ(vsl::str_to_numeric<double>("1.25"), 1.25);
    EXPECT_EQ(vsl::str_to_numeric<double>("-123.5"), -123.5);
    EXPECT_EQ(vsl::str_to_numeric<double>("1.25e10"), 1.25e10);
    EXPECT_EQ(vsl::str_to_numeric<double>("-1.25e-10"), -1.25e-10);

    EXPECT_EQ(vsl::str_to_numeric<float>("1.25"), 1.25F);
}

TEST(ConvertTest, StrToNumericBoolDefaultAlpha)
{
    EXPECT_FALSE(vsl::str_to_numeric<bool>("false"));
    EXPECT_FALSE(vsl::str_to_numeric<bool>("False"));
    EXPECT_FALSE(vsl::str_to_numeric<bool>("FALSE"));

    EXPECT_TRUE(vsl::str_to_numeric<bool>("true"));
    EXPECT_TRUE(vsl::str_to_numeric<bool>("True"));
    EXPECT_TRUE(vsl::str_to_numeric<bool>("TRUE"));
}

TEST(ConvertTest, StrToNumericBoolDefaultIntegral)
{
    EXPECT_FALSE(vsl::str_to_numeric<bool>("0"));

    EXPECT_TRUE(vsl::str_to_numeric<bool>("1"));
    EXPECT_TRUE(vsl::str_to_numeric<bool>("2"));
    EXPECT_TRUE(vsl::str_to_numeric<bool>("-1"));
    EXPECT_TRUE(vsl::str_to_numeric<bool>("123"));
    EXPECT_TRUE(vsl::str_to_numeric<bool>("-123"));

    EXPECT_TRUE(vsl::str_to_numeric<bool>("-9223372036854775808"));
    EXPECT_TRUE(vsl::str_to_numeric<bool>("9223372036854775807"));
}

TEST(ConvertTest, StrToNumericBoolAlphaOnly)
{
    EXPECT_FALSE((vsl::str_to_numeric<bool, BOOL_ALPHA_ONLY>("false")));
    EXPECT_FALSE((vsl::str_to_numeric<bool, BOOL_ALPHA_ONLY>("False")));
    EXPECT_FALSE((vsl::str_to_numeric<bool, BOOL_ALPHA_ONLY>("FALSE")));

    EXPECT_TRUE((vsl::str_to_numeric<bool, BOOL_ALPHA_ONLY>("true")));
    EXPECT_TRUE((vsl::str_to_numeric<bool, BOOL_ALPHA_ONLY>("True")));
    EXPECT_TRUE((vsl::str_to_numeric<bool, BOOL_ALPHA_ONLY>("TRUE")));

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool, BOOL_ALPHA_ONLY>("0")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool, BOOL_ALPHA_ONLY>("1")), vsl::ConversionError);
}

TEST(ConvertTest, StrToNumericBoolZeroOneOnly)
{
    EXPECT_FALSE((vsl::str_to_numeric<bool, BOOL_0_1_ONLY>("0")));
    EXPECT_TRUE((vsl::str_to_numeric<bool, BOOL_0_1_ONLY>("1")));

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool, BOOL_0_1_ONLY>("-1")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool, BOOL_0_1_ONLY>("2")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool, BOOL_0_1_ONLY>("true")), vsl::ConversionError);
}

TEST(ConvertTest, StrToNumericBoolZeroNonzeroOnly)
{
    EXPECT_FALSE((vsl::str_to_numeric<bool, BOOL_0_NE0_ONLY>("0")));

    EXPECT_TRUE((vsl::str_to_numeric<bool, BOOL_0_NE0_ONLY>("1")));
    EXPECT_TRUE((vsl::str_to_numeric<bool, BOOL_0_NE0_ONLY>("-1")));
    EXPECT_TRUE((vsl::str_to_numeric<bool, BOOL_0_NE0_ONLY>("123")));
    EXPECT_TRUE((vsl::str_to_numeric<bool, BOOL_0_NE0_ONLY>("-123")));

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool, BOOL_0_NE0_ONLY>("true")), vsl::ConversionError);
}

TEST(ConvertTest, StrToNumericBoolAllConversionsDisabled)
{
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool, BOOL_NONE>("false")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool, BOOL_NONE>("true")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool, BOOL_NONE>("0")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool, BOOL_NONE>("1")), vsl::ConversionError);
}

TEST(ConvertTest, StrToNumericLeadingPlus)
{
    EXPECT_EQ(vsl::str_to_numeric<int>("+123"), 123);
    EXPECT_EQ(vsl::str_to_numeric<double>("+1.25"), 1.25);
    EXPECT_EQ(vsl::str_to_numeric<bool>("+123"), true);

    EXPECT_EQ(vsl::str_to_numeric<unsigned int>("+123"), 123u);
    EXPECT_EQ(vsl::str_to_numeric<u64>("+18446744073709551615"), vsl::U64_MAX);

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<int>("++123")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<double>("++1.25")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool>("++123")), vsl::ConversionError);

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<int>(" +123")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<double>(" +1.25")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool>(" +123")), vsl::ConversionError);

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<int>("+ 123")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<double>("+ 1.25")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool>("+ 123")), vsl::ConversionError);

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<int>("+-123")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<double>("+-1.25")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool>("+-123")), vsl::ConversionError);

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<int>("+")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<double>("+")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool>("+")), vsl::ConversionError);

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<int>("+123abc")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<double>("+1.25abc")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<bool>("+123abc")), vsl::ConversionError);

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<i64>("+9223372036854775808")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<u64>("+18446744073709551616")), vsl::ConversionError);
}

TEST(ConvertTest, StrToNumericRejectsEmptyString)
{
    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<int>("")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("invalid argument")));

    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<double>("")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("invalid argument")));

    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<bool>("")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("invalid argument")));
}

TEST(ConvertTest, StrToNumericRejectsInvalidArgument)
{
    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<int>("abc")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("invalid argument")));

    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<double>("abc")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("invalid argument")));

    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<bool>("abc")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("invalid argument")));
}

TEST(ConvertTest, StrToNumericRejectsExtraTrailingCharacters)
{
    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<int>("123abc")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("extra trailing characters")));

    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<double>("1.25abc")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("extra trailing characters")));

    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<bool>("123abc")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("extra trailing characters")));
}

TEST(ConvertTest, StrToNumericRejectsOutOfRange)
{
    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<i64>("9223372036854775808")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("out of range")));

    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<i64>("-9223372036854775809")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("out of range")));

    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<u64>("18446744073709551616")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("out of range")));

    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<double>("1e9999")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("out of range")));

    EXPECT_THAT([] { static_cast<void>(vsl::str_to_numeric<bool>("9223372036854775808")); },
                ThrowsMessage<vsl::ConversionError>(HasSubstr("out of range")));
}

TEST(ConvertTest, StrToNumericRejectsWhitespace)
{
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<int>(" 123")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<int>("123 ")), vsl::ConversionError);

    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<double>(" 1.25")), vsl::ConversionError);
    EXPECT_THROW(static_cast<void>(vsl::str_to_numeric<double>("1.25 ")), vsl::ConversionError);
}

TEST(ConvertTest, NumericConversionRoundTrip)
{
    EXPECT_EQ(vsl::str_to_numeric<int>(vsl::numeric_to_str(123)), 123);
    EXPECT_EQ(vsl::str_to_numeric<int>(vsl::numeric_to_str(-123)), -123);

    EXPECT_EQ(vsl::str_to_numeric<i64>(vsl::numeric_to_str(vsl::I64_MIN)), vsl::I64_MIN);
    EXPECT_EQ(vsl::str_to_numeric<u64>(vsl::numeric_to_str(vsl::U64_MAX)), vsl::U64_MAX);

    EXPECT_EQ(vsl::str_to_numeric<double>(vsl::numeric_to_str(1.25)), 1.25);

    EXPECT_FALSE(vsl::str_to_numeric<bool>(vsl::numeric_to_str(false)));
    EXPECT_TRUE(vsl::str_to_numeric<bool>(vsl::numeric_to_str(true)));
}

}  // namespace test
