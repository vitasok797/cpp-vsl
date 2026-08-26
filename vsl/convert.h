#ifndef VSL_CONVERT_H
#define VSL_CONVERT_H

#include <vsl/concepts.h>
#include <vsl/reflection.h>
#include <vsl/result.h>
#include <vsl/types.h>
#include <vsl/util.h>

#include <fmt/format.h>

#include <charconv>
#include <concepts>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

namespace vsl
{

class ConversionError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct NumericToStrFormat
{
    std::string boolean{"{}"};
    std::string integral{"{}"};
    std::string floating{"{}"};
};

namespace detail
{

template<typename T>
concept conversion_numeric = (vsl::strict_numeric<T> || std::same_as<T, bool>) && vsl::not_cvref<T>;

}  // namespace detail

template<detail::conversion_numeric Input>
inline auto numeric_to_str(std::string& out, Input value, const NumericToStrFormat& format = {}) -> void
{
    if constexpr (std::same_as<Input, bool>)
    {
        fmt::format_to(std::back_inserter(out), fmt::runtime(format.boolean), value);
    }
    else if constexpr (std::integral<Input>)
    {
        fmt::format_to(std::back_inserter(out), fmt::runtime(format.integral), value);
    }
    else if constexpr (std::floating_point<Input>)
    {
        fmt::format_to(std::back_inserter(out), fmt::runtime(format.floating), value);
    }
    else
    {
        static_assert(false, "Unknown type");
    }
}

template<detail::conversion_numeric Input>
[[nodiscard]]
inline auto numeric_to_str(Input value, const NumericToStrFormat& format = {}) -> std::string
{
    auto res = std::string{};
    numeric_to_str(res, value, format);
    return res;
}

namespace detail
{

template<vsl::strict_numeric Output>
    requires(vsl::not_cvref<Output>)
[[nodiscard]]
inline auto str_to_numeric_raw(std::string_view str) -> vsl::Result<Output, std::string>
{
    constexpr auto INVALID_ARGUMENT_ERROR = "invalid argument";
    constexpr auto OUT_OF_RANGE_ERROR = "out of range";
    constexpr auto EXTRA_TRAILING_CHARACTERS_ERROR = "extra trailing characters";

    if (str.empty())
    {
        return vsl::ResultError{INVALID_ARGUMENT_ERROR};
    }

    if (str.starts_with('+'))
    {
        str.remove_prefix(1);

        if (str.empty() || str.starts_with('+') || str.starts_with('-'))
        {
            return vsl::ResultError{INVALID_ARGUMENT_ERROR};
        }
    }

    auto output = Output{};

    const auto str_begin = str.data();
    const auto str_end = str.data() + str.size();
    const auto [ptr, ec] = std::from_chars(str_begin, str_end, output);

    if (ec == std::errc::invalid_argument) [[unlikely]]
    {
        return vsl::ResultError{INVALID_ARGUMENT_ERROR};
    }

    if (ec == std::errc::result_out_of_range) [[unlikely]]
    {
        return vsl::ResultError{OUT_OF_RANGE_ERROR};
    }

    const auto chars_left = (ptr != str_end);
    if (chars_left) [[unlikely]]
    {
        return vsl::ResultError{EXTRA_TRAILING_CHARACTERS_ERROR};
    }

    return output;
}

[[noreturn]]
inline auto throw_str_to_numeric_conversion_error(std::string_view str,
                                                  std::string_view type_name,
                                                  std::string_view raw_error = {})
{
    auto msg = fmt::format("Unable to convert {:?} to type <{}>", str, type_name);
    if (!raw_error.empty())
    {
        fmt::format_to(std::back_inserter(msg), " ({})", raw_error);
    }
    throw ConversionError{msg};
}

}  // namespace detail

using BoolIntegralRange = i64;

struct StrToNumericOptions
{
    bool bool_integral_0_1{false};   // 0, 1
    bool bool_integral_0_ne0{true};  // 0, !=0 (value must be in range of BoolIntegralRange)
    bool bool_alpha{true};           // "false"/"False"/"FALSE", "true"/"True"/"TRUE"
};

template<detail::conversion_numeric Output, StrToNumericOptions options = {}>
[[nodiscard]]
inline auto str_to_numeric(std::string_view str) -> Output
{
    if constexpr (std::same_as<Output, bool>)
    {
        if constexpr (options.bool_alpha)
        {
            if (vsl::is_one_of(str, "false", "False", "FALSE")) return false;
            if (vsl::is_one_of(str, "true", "True", "TRUE")) return true;
        }

        if constexpr (options.bool_integral_0_1 || options.bool_integral_0_ne0)
        {
            const auto res = detail::str_to_numeric_raw<BoolIntegralRange>(str);
            if (res)
            {
                if (res.value() == 0) return false;
                if (res.value() == 1) return true;
                if (res.value() != 0 && options.bool_integral_0_ne0) return true;
            }
            else [[unlikely]]
            {
                detail::throw_str_to_numeric_conversion_error(str, "bool", res.error());
            }
        }

        detail::throw_str_to_numeric_conversion_error(str, "bool");
    }
    else
    {
        const auto res = detail::str_to_numeric_raw<Output>(str);
        if (res)
        {
            return res.value();
        }
        else [[unlikely]]
        {
            detail::throw_str_to_numeric_conversion_error(str, NAMEOF_TYPE(Output), res.error());
        }
    }
}

}  // namespace vsl

#endif  // VSL_CONVERT_H
