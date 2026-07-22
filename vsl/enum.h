#ifndef VSL_ENUM_H
#define VSL_ENUM_H

#include <vsl/types.h>

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_switch.hpp>

#include <optional>
#include <string_view>
#include <type_traits>

// Reference:
// https://github.com/Neargye/magic_enum/blob/master/doc/reference.md

#define VSL_DECLARE_ENUM_FLAGS(EnumType)                                           \
    static_assert(std::is_unsigned_v<std::underlying_type_t<EnumType>>,            \
                  "VSL_DECLARE_ENUM_FLAGS requires an unsigned underlying type");  \
    constexpr auto operator|(EnumType lhs, EnumType rhs) noexcept -> EnumType      \
    {                                                                              \
        using U = std::underlying_type_t<EnumType>;                                \
        return static_cast<EnumType>(static_cast<U>(lhs) | static_cast<U>(rhs));   \
    }                                                                              \
    constexpr auto operator&(EnumType lhs, EnumType rhs) noexcept -> EnumType      \
    {                                                                              \
        using U = std::underlying_type_t<EnumType>;                                \
        return static_cast<EnumType>(static_cast<U>(lhs) & static_cast<U>(rhs));   \
    }                                                                              \
    constexpr auto operator~(EnumType val) noexcept -> EnumType                    \
    {                                                                              \
        using U = std::underlying_type_t<EnumType>;                                \
        return static_cast<EnumType>(~static_cast<U>(val));                        \
    }                                                                              \
    constexpr auto operator|=(EnumType& lhs, EnumType rhs) noexcept -> EnumType&   \
    {                                                                              \
        return lhs = lhs | rhs;                                                    \
    }

namespace vsl
{

// TODO: Add functions:
// * enum_flags_*
// * containers::bitset

using magic_enum::enum_constant;
using magic_enum::enum_entries;
using magic_enum::enum_integer;
using magic_enum::enum_name;
using magic_enum::enum_names;
using magic_enum::enum_switch;
using magic_enum::enum_type_name;
using magic_enum::enum_values;

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_count() noexcept -> vsl::Index
{
    return static_cast<vsl::Index>(magic_enum::enum_count<E>());
}

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_index(E value) noexcept -> std::optional<vsl::Index>
{
    const auto index = magic_enum::enum_index(value);
    if (index.has_value())
    {
        return static_cast<vsl::Index>(index.value());
    }
    else
    {
        return std::nullopt;
    }
}

template<auto V>
    requires std::is_enum_v<std::remove_cvref_t<decltype(V)>>
constexpr auto enum_index() noexcept -> vsl::Index
{
    return static_cast<vsl::Index>(magic_enum::enum_index<V>());
}

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_contains(E value) noexcept -> bool
{
    return magic_enum::enum_contains<E>(value);
}

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_contains(std::underlying_type_t<E> value) noexcept -> bool
{
    return magic_enum::enum_contains<E>(value);
}

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_contains(std::string_view value) noexcept -> bool
{
    return magic_enum::enum_contains<E>(value);
}

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_contains_icase(std::string_view value) noexcept -> bool
{
    return magic_enum::enum_contains<E>(value, magic_enum::case_insensitive);
}

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_cast(std::underlying_type_t<E> value) noexcept -> std::optional<E>
{
    return magic_enum::enum_cast<E>(value);
}

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_from_string(std::string_view value) noexcept -> std::optional<E>
{
    return magic_enum::enum_cast<E>(value);
}

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_from_string_icase(std::string_view value) noexcept -> std::optional<E>
{
    return magic_enum::enum_cast<E>(value, magic_enum::case_insensitive);
}

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_contains_flags(E e, E flags) noexcept -> bool
{
    using U = std::underlying_type_t<E>;
    const auto mask = static_cast<U>(flags);
    return (mask != U{0}) && ((static_cast<U>(e) & mask) == mask);
}

}  // namespace vsl

#endif  // VSL_ENUM_H
