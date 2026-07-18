#ifndef VSL_UTIL_H
#define VSL_UTIL_H

#include <vsl/concepts.h>
#include <vsl/types.h>

#include <algorithm>
#include <initializer_list>
#include <ranges>
#include <type_traits>
#include <utility>

#define FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

namespace vsl
{

template<typename T>
constexpr auto signed_size(const T& t) noexcept -> auto
{
    return vsl::as_signed(t.size());
}

// C++23: Use std::forward_like
template<typename T, typename U>
constexpr auto forward_like(U&& x) noexcept -> auto&&
{
    constexpr bool is_adding_const = std::is_const_v<std::remove_reference_t<T>>;
    if constexpr (std::is_lvalue_reference_v<T&&>)
    {
        if constexpr (is_adding_const)
            return std::as_const(x);
        else
            return static_cast<U&>(x);
    }
    else
    {
        if constexpr (is_adding_const)
            return std::move(std::as_const(x));
        else
            return std::move(x);
    }
}

template<typename T, typename... Options>
auto is_one_of(const T& value, const Options&... options) -> bool
{
    return ((value == options) || ...);
}

template<typename T>
auto is_one_of(const T& value, const std::initializer_list<T>& options) -> bool
{
    return std::ranges::find(options, value) != options.end();
}

template<typename T, std::ranges::input_range R>
    requires vsl::range_of<R, T>
auto is_one_of(const T& value, const R& options) -> bool
{
    return std::ranges::find(options, value) != options.end();
}

template<typename... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

// CRTP underlying type casting helper
template<typename Und, typename Ptr>
auto this_to(Ptr* this_ptr) noexcept -> decltype(auto)
{
    using input_type = std::remove_pointer_t<Ptr>;
    using output_type = std::conditional_t<std::is_const_v<input_type>, const Und&, Und&>;
    return static_cast<output_type>(*this_ptr);
}

}  // namespace vsl

#endif  // VSL_UTIL_H
