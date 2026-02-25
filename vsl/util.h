#ifndef VSL_UTIL_H
#define VSL_UTIL_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#define FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

namespace vsl
{

using cstring = const char*;

using Index = std::ptrdiff_t;
using SignedSize = std::ptrdiff_t;

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

template<typename T>
constexpr auto signed_size(const T& t) noexcept -> auto
{
    return as_signed(t.size());
}

template<typename T>
using optional_ref = std::optional<std::reference_wrapper<T>>;

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
bool is_one_of(const T& value, const Options&... options)
{
    return ((value == options) || ...);
}

template<typename T>
bool is_one_of(const T& value, const std::initializer_list<T>& options)
{
    return std::ranges::find(options, value) != options.end();
}

template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

struct StringHash
{
    using is_transparent = void;

    std::size_t operator()(const char* str) const
    {
        return std::hash<std::string_view>{}(str);
    }

    std::size_t operator()(std::string_view str) const
    {
        return std::hash<std::string_view>{}(str);
    }

    std::size_t operator()(const std::string& str) const
    {
        return std::hash<std::string>{}(str);
    }
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
