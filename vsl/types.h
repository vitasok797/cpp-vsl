#ifndef VSL_TYPES_H
#define VSL_TYPES_H

#include <vsl/concepts.h>

#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <limits>
#include <optional>
#include <type_traits>
#include <utility>

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using cstring = const char*;

namespace vsl
{

inline constexpr auto I8_MIN = std::numeric_limits<i8>::min();
inline constexpr auto I8_MAX = std::numeric_limits<i8>::max();
inline constexpr auto I16_MIN = std::numeric_limits<i16>::min();
inline constexpr auto I16_MAX = std::numeric_limits<i16>::max();
inline constexpr auto I32_MIN = std::numeric_limits<i32>::min();
inline constexpr auto I32_MAX = std::numeric_limits<i32>::max();
inline constexpr auto I64_MIN = std::numeric_limits<i64>::min();
inline constexpr auto I64_MAX = std::numeric_limits<i64>::max();

inline constexpr auto U8_MIN = std::numeric_limits<u8>::min();
inline constexpr auto U8_MAX = std::numeric_limits<u8>::max();
inline constexpr auto U16_MIN = std::numeric_limits<u16>::min();
inline constexpr auto U16_MAX = std::numeric_limits<u16>::max();
inline constexpr auto U32_MIN = std::numeric_limits<u32>::min();
inline constexpr auto U32_MAX = std::numeric_limits<u32>::max();
inline constexpr auto U64_MIN = std::numeric_limits<u64>::min();
inline constexpr auto U64_MAX = std::numeric_limits<u64>::max();

inline constexpr auto DOUBLE_MAX = std::numeric_limits<double>::max();
inline constexpr auto DOUBLE_MAX_NEG = std::numeric_limits<double>::lowest();
inline constexpr auto INF = std::numeric_limits<double>::infinity();

using Index = std::ptrdiff_t;
using SignedSize = std::ptrdiff_t;

template<typename T>
using OptionalRef = std::optional<std::reference_wrapper<T>>;

template<typename T>
constexpr auto as_signed(T t) noexcept -> auto
{
    return std::make_signed_t<T>(t);
}

template<typename T>
constexpr auto as_unsigned(T t) noexcept -> auto
{
    return std::make_unsigned_t<T>(t);
}

struct NarrowingError : public std::exception
{
    auto what() const noexcept -> const char* override
    {
        return "Narrowing error";
    }
};

// Implementation from GSL
// https://github.com/microsoft/GSL/blob/main/include/gsl/narrow
template<vsl::numeric T, vsl::numeric U>
constexpr auto numeric_cast(U u) -> T
{
    constexpr auto is_different_signedness = (std::is_signed_v<T> != std::is_signed_v<U>);
    const auto t = static_cast<T>(u);

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
    // Note: NaN will always throw, since NaN != NaN
    if (static_cast<U>(t) != u || (is_different_signedness && ((t < T{}) != (u < U{}))))
    {
        throw NarrowingError{};
    }
#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    return t;
}

template<typename T, typename U>
constexpr auto narrow_cast(U&& u) noexcept -> T
{
    return static_cast<T>(std::forward<U>(u));
}

}  // namespace vsl

#endif  // VSL_TYPES_H
