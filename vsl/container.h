#ifndef VSL_CONTAINER_H
#define VSL_CONTAINER_H

#include <cstddef>
#include <string>
#include <string_view>

namespace vsl
{

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

}  // namespace vsl

#endif  // VSL_CONTAINER_H
