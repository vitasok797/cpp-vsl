#ifndef VSL_CONTAINER_H
#define VSL_CONTAINER_H

#include <vsl/types.h>

#include <algorithm>
#include <string>
#include <string_view>

namespace vsl
{

struct StringHash
{
    using is_transparent = void;

    auto operator()(const char* str) const -> std::size_t
    {
        return std::hash<std::string_view>{}(str);
    }

    auto operator()(std::string_view str) const -> std::size_t
    {
        return std::hash<std::string_view>{}(str);
    }

    auto operator()(const std::string& str) const -> std::size_t
    {
        return std::hash<std::string>{}(str);
    }
};

struct StringAsciiIcaseCompare
{
    using is_transparent = void;

    constexpr bool operator()(std::string_view a, std::string_view b) const noexcept
    {
        auto to_lower = [](unsigned char c) constexpr noexcept -> unsigned char
        { return (c >= 'A' && c <= 'Z') ? static_cast<unsigned char>(c + ('a' - 'A')) : c; };

        return std::ranges::lexicographical_compare(a, b, std::ranges::less{}, to_lower, to_lower);
    }
};

template<typename T>
auto get_hash_table_fill_efficiency(const T& container) -> double
{
    if (container.empty()) return 1.0;

    auto occupied_buckets = 0;
    for (auto i = 0; i < vsl::as_signed(container.bucket_count()); ++i)
    {
        if (container.bucket_size(i) > 0)
        {
            occupied_buckets++;
        }
    }

    return static_cast<double>(occupied_buckets) / static_cast<double>(container.size());
}

}  // namespace vsl

#endif  // VSL_CONTAINER_H
