#ifndef VSL_ASSERT_H
#define VSL_ASSERT_H

#include <fmt/format.h>

#include <source_location>
#include <stdexcept>

#define VSL_ASSERT(type, expr, ...) (static_cast<bool>(expr) ? static_cast<void>(0) : \
    vsl::detail::assert_fail(type, #expr, std::source_location::current() __VA_OPT__(,) __VA_ARGS__))

#define VSL_EXPECTS(...) VSL_ASSERT("Expects", __VA_ARGS__)
#define VSL_ENSURES(...) VSL_ASSERT("Ensures", __VA_ARGS__)

namespace vsl
{

class AssertionError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

namespace detail
{

[[noreturn]]
inline auto assert_fail(                   //
    const char* type,                      //
    const char* expr,                      //
    const std::source_location& location,  //
    const char* desc = nullptr             //
    ) -> void                              //
{
    const auto msg = fmt::format("Assertion failed\n{}: {}\nDescription: {}\nLocation: {}:{}\nFunction: {}", type, expr,
                                 desc ? desc : "n/a", location.file_name(), location.line(), location.function_name());
    throw AssertionError{msg};
}

}  // namespace detail
}  // namespace vsl

#endif  // VSL_ASSERT_H
