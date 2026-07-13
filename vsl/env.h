#ifndef VSL_ENV_H
#define VSL_ENV_H

#include <vsl/concepts.h>

#include <libenvpp/env.hpp>

#include <string>
#include <string_view>
#include <utility>

// Reference:
// https://github.com/ph3at/libenvpp

namespace vsl
{

// TODO: Add functions:
// * prefix
// * register_option
// * ...

class EnvError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class EnvNotFound : public EnvError
{
    using EnvError::EnvError;
};

class EnvParseError : public EnvError
{
    using EnvError::EnvError;
};

inline auto env_exists(std::string_view var) -> bool
{
    return env::get<std::string>(var).has_value();
}

template<typename Value>
    requires(!vsl::same_type_as<Value, std::string_view>)
auto env_get(std::string_view var) -> auto
{
    const auto res = env::get<Value>(var);

    if (!res.has_value())
    {
        if (env_exists(var))
        {
            throw EnvParseError{res.error().what()};
        }
        else
        {
            throw EnvNotFound{res.error().what()};
        }
    }

    return res.value();
}

template<typename Value, typename DefaultValue = Value>
    requires(!vsl::same_type_as<Value, std::string_view>)
auto env_get_or(std::string_view var, DefaultValue&& default_value) -> Value
{
    return env::get_or<Value, DefaultValue>(var, std::forward<DefaultValue>(default_value));
}

}  // namespace vsl

#endif  // VSL_ENV_H
