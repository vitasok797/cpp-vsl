#ifndef VSL_CONTAINER_H
#define VSL_CONTAINER_H

#include <vsl/util.h>

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

namespace vsl
{

namespace detail
{

inline constexpr auto view_as_rvalue_disp = vsl::overloaded{[](const auto& x) -> decltype(auto) { return x; },       //
                                                            [](auto& x) -> decltype(auto) { return std::move(x); },  //
                                                            [](auto&& x) -> auto { return std::move(x); }};          //

}  // namespace detail

// NOTE: If targeting C++23, prefer std::views::as_rvalue
inline constexpr auto view_as_rvalue = std::views::transform(detail::view_as_rvalue_disp);

struct StringHash
{
    using is_transparent = void;

    auto operator()(const char* str) const -> size_t
    {
        return std::hash<std::string_view>{}(str);
    }

    auto operator()(std::string_view str) const -> size_t
    {
        return std::hash<std::string_view>{}(str);
    }

    auto operator()(const std::string& str) const -> size_t
    {
        return std::hash<std::string>{}(str);
    }
};

struct StringAsciiCaselessCompare
{
    using is_transparent = void;

    constexpr auto operator()(std::string_view a, std::string_view b) const noexcept -> bool
    {
        constexpr auto char_to_lower = [](char c) noexcept -> unsigned char
        {
            const auto uc = static_cast<unsigned char>(c);
            return (uc >= 'A' && uc <= 'Z') ? static_cast<unsigned char>(uc + ('a' - 'A')) : uc;
        };

        return std::ranges::lexicographical_compare(a, b, {}, char_to_lower, char_to_lower);
    }
};

template<typename T>
[[nodiscard]]
inline auto get_hash_table_fill_efficiency(const T& container) -> double
{
    if (container.empty()) return 1.0;

    auto occupied_buckets = size_t{0};
    for (auto i = size_t{0}; i < container.bucket_count(); ++i)
    {
        if (container.bucket_size(i) > 0)
        {
            ++occupied_buckets;
        }
    }

    return static_cast<double>(occupied_buckets) / static_cast<double>(container.size());
}

}  // namespace vsl

#endif  // VSL_CONTAINER_H
