#ifndef VSL_ENUM_H
#define VSL_ENUM_H

#include <vsl/types.h>

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_switch.hpp>

#include <optional>
#include <string_view>
#include <type_traits>

namespace vsl
{

// Reference:
// https://github.com/Neargye/magic_enum/blob/master/doc/reference.md

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
constexpr auto enum_count() noexcept -> SignedSize
{
    return narrow_cast<SignedSize>(magic_enum::enum_count<E>());
}

template<typename E>
    requires std::is_enum_v<E>
constexpr auto enum_index(E value) noexcept -> std::optional<Index>
{
    const auto index = magic_enum::enum_index(value);
    if (index.has_value())
    {
        return narrow_cast<Index>(index.value());
    }
    else
    {
        return std::nullopt;
    }
}

template<auto V>
    requires std::is_enum_v<std::decay_t<decltype(V)>>
constexpr auto enum_index() noexcept -> Index
{
    return narrow_cast<Index>(magic_enum::enum_index<V>());
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

}  // namespace vsl

#endif  // VSL_ENUM_H
