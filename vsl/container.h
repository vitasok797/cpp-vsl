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

template<typename T>
auto get_hash_table_fill_efficiency(const T& container) -> double
{
    if (container.empty()) return 1.0;

    auto occupied_buckets = 0;
    for (auto i = 0; i < container.bucket_count(); ++i)
    {
        if (container.bucket_size(i) > 0)
        {
            occupied_buckets++;
        }
    }

    return static_cast<double>(occupied_buckets) / container.size();
}

}  // namespace vsl

#endif  // VSL_CONTAINER_H
