#ifndef VSL_ENCODINGS_DETAIL_H
#define VSL_ENCODINGS_DETAIL_H

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace vsl::encodings::detail
{

template<typename CodepointType, size_t ReplSize, size_t MapSize>
using ToEncodingMap = std::array<std::pair<CodepointType, std::array<unsigned char, ReplSize>>, MapSize>;

template<size_t ReplSize>
using FromEncodingMap = std::array<std::array<unsigned char, ReplSize>, 128>;

template<typename MapType>
consteval auto is_to_encoding_map_sorted(const MapType& map) -> bool
{
    using MapPairType = typename MapType::value_type;
    return std::ranges::is_sorted(map, {}, &MapPairType::first);
}

template<typename Encoding>
using ToEncodingMapCodepointType =
    typename std::remove_cvref_t<decltype(Encoding::to_encoding_map)>::value_type::first_type;

template<typename Encoding>
concept encoding_to = requires(const Encoding& encoding, std::string& s, std::string_view sv) {
    Encoding::to_encoding_map;
    requires is_to_encoding_map_sorted(Encoding::to_encoding_map);
    requires std::unsigned_integral<ToEncodingMapCodepointType<Encoding>>;
    requires Encoding::to_encoding_map.size() <= 128;
    { encoding.to_encoding_reserve(s, sv) } -> std::same_as<void>;
};

template<typename Encoding>
concept encoding_from = requires(const Encoding& encoding, std::string& s, std::string_view sv) {
    Encoding::from_encoding_map;
    { encoding.from_encoding_reserve(s, sv) } -> std::same_as<void>;
};

}  // namespace vsl::encodings::detail

#endif  // VSL_ENCODINGS_DETAIL_H
