#ifndef VSL_POINTER_H
#define VSL_POINTER_H

#include <exception>
#include <optional>

namespace vsl
{

struct NullPointerError : public std::exception
{
    auto what() const noexcept -> const char* override
    {
        return "Null pointer error";
    }
};

template<typename T>
auto check_ptr(const T& ptr) -> void
{
    if (!ptr) throw NullPointerError{};
}

template<typename T>
auto checked_deref_ptr(const T& ptr) -> decltype(auto)
{
    if (!ptr) throw NullPointerError{};
    return *ptr;
}

template<typename T>
auto checked_get_ptr(const T& ptr) -> decltype(auto)
{
    if (!ptr) throw NullPointerError{};
    return ptr.get();
}

}  // namespace vsl

template<typename T>
constexpr auto as_ptr(std::optional<T>& opt) noexcept -> T*
{
    return opt.has_value() ? &(*opt) : nullptr;
}

template<typename T>
constexpr auto as_ptr(const std::optional<T>& opt) noexcept -> const T*
{
    return opt.has_value() ? &(*opt) : nullptr;
}

#endif  // VSL_POINTER_H
