#ifndef VSL_CONCEPTS_H
#define VSL_CONCEPTS_H

#include <concepts>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>

namespace vsl
{

template<typename T>
concept numeric = std::integral<std::remove_cvref_t<T>> || std::floating_point<std::remove_cvref_t<T>>;

template<typename T, typename U>
concept same_type_as = std::same_as<std::decay_t<T>, std::decay_t<U>>;

template<typename T, typename... Types>
concept one_of_type = (std::same_as<std::decay_t<T>, Types> || ...);

template<typename T>
concept string_like = one_of_type<T, std::string, std::string_view, const char*, char*>;

template<typename R, typename T>
concept range_of = std::same_as<std::ranges::range_value_t<R>, std::remove_cvref_t<T>>;

template<typename R, typename T>
concept range_of_convertible_to = std::convertible_to<std::ranges::range_value_t<R>, T>;

template<typename R>
concept range_of_string_like = string_like<std::ranges::range_value_t<R>>;

template<typename R, typename T>
concept range_view_of = range_of<R, T> && std::ranges::view<R>;

template<typename R, typename T>
concept range_view_of_convertible_to = range_of_convertible_to<R, T> && std::ranges::view<R>;

template<typename R>
concept range_view_of_string_like = range_of_string_like<R> && std::ranges::view<R>;

template<typename T>
concept strict_signed_integral = std::signed_integral<T> && !one_of_type<T, char, wchar_t, char8_t, char16_t, char32_t>;

template<typename T>
concept strict_unsigned_integral =
    std::unsigned_integral<T> && !one_of_type<T, bool, char, wchar_t, char8_t, char16_t, char32_t>;

}  // namespace vsl

#endif  // VSL_CONCEPTS_H
