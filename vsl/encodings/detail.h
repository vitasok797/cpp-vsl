#ifndef VSL_ENCODINGS_DETAIL_H
#define VSL_ENCODINGS_DETAIL_H

#include <array>
#include <cstdint>
#include <string_view>
#include <utility>

namespace vsl::encodings::detail
{

using Codepoint = uint32_t;
using ToEncodingMapItem = std::pair<Codepoint, std::string_view>;
using FromEncodingMap = std::array<std::string_view, 128>;

inline constexpr auto UTF8_REPL_CHAR = "\xEF\xBF\xBD";

}  // namespace vsl::encodings::detail

#endif  // VSL_ENCODINGS_DETAIL_H
