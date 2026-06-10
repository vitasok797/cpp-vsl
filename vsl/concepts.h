#ifndef VSL_CONCEPTS_H
#define VSL_CONCEPTS_H

#include <concepts>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>

namespace vsl
{

template<typename Fn, typename... Args>
concept callable = std::is_invocable_v<Fn, Args...>;

template<typename Ret, typename Fn, typename... Args>
concept callable_r = std::is_invocable_r_v<Ret, Fn, Args...>;

template<typename T>
concept numeric = std::integral<std::decay_t<T>> || std::floating_point<std::decay_t<T>>;

template<typename T, typename U>
concept same_type_as = std::same_as<std::decay_t<T>, std::decay_t<U>>;

template<typename T, typename... Types>
concept one_of = (std::same_as<std::decay_t<T>, Types> || ...);

template<typename T>
concept string_like = one_of<T, std::string, std::string_view, const char*, char*>;

template<typename R, typename T>
concept range_of = std::same_as<std::ranges::range_value_t<R>, std::decay_t<T>>;

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

}  // namespace vsl

#endif  // VSL_CONCEPTS_H
