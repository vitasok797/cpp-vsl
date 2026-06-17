#ifndef VSL_UNICODE_H
#define VSL_UNICODE_H

#include <uni_algo/conv.h>
#include <uni_algo/norm.h>

#include <string>
#include <string_view>

namespace vsl
{

using una::is_valid_utf16;
using una::is_valid_utf8;

using una::norm::to_nfc_utf8;
using una::norm::to_nfd_utf8;
using una::norm::to_nfkc_utf8;
using una::norm::to_nfkd_utf8;

using una::norm::is_nfc_utf8;
using una::norm::is_nfd_utf8;
using una::norm::is_nfkc_utf8;
using una::norm::is_nfkd_utf8;

[[nodiscard]]
inline auto utf16to8(std::u16string_view str) -> std::string
{
    return una::utf16to8<char16_t, char>(str);
}

[[nodiscard]]
inline auto utf8to16(std::string_view str) -> std::u16string
{
    return una::utf8to16<char, char16_t>(str);
}

}  // namespace vsl

#endif  // VSL_UNICODE_H
