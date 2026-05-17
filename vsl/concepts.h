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

template<typename T, typename... Types>
concept one_of = (std::same_as<std::decay_t<T>, Types> || ...);

template<typename T, typename U>
concept same_type_as = std::same_as<std::decay_t<T>, U>;

template<typename T>
concept string_like = one_of<T, std::string, std::string_view, const char*, char*>;

template<typename T>
concept string_input_range = std::ranges::input_range<T> && string_like<std::ranges::range_value_t<T>>;

template<typename T>
concept string_forward_range = std::ranges::forward_range<T> && string_like<std::ranges::range_value_t<T>>;

template<typename T>
concept string_bidirectional_range = std::ranges::bidirectional_range<T> && string_like<std::ranges::range_value_t<T>>;

template<typename T>
concept string_random_access_range = std::ranges::random_access_range<T> && string_like<std::ranges::range_value_t<T>>;

}  // namespace vsl

#endif  // VSL_CONCEPTS_H
