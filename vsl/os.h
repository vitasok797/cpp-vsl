#ifndef VSL_OS_H
#define VSL_OS_H

#if defined(linux) || defined(__linux) || defined(__linux__)
#define VSL_LINUX_OS
#elif defined(_WIN32) || defined(_WIN64)
#define VSL_WINDOWS_OS
#else
#error Unknown OS
#endif

#if defined(VSL_WINDOWS_OS)
#define VSL_NATIVE_NEWLINE "\r\n"
#else
#define VSL_NATIVE_NEWLINE "\n"
#endif

namespace vsl
{

enum class OsFamily
{
    LINUX,
    WINDOWS,
};

#if defined(VSL_LINUX_OS)
inline constexpr auto OS = OsFamily::LINUX;
inline constexpr auto IS_LINUX = true;
#else
inline constexpr auto IS_LINUX = false;
#endif

#if defined(VSL_WINDOWS_OS)
inline constexpr auto OS = OsFamily::WINDOWS;
inline constexpr auto IS_WINDOWS = true;
#else
inline constexpr auto IS_WINDOWS = false;
#endif

inline constexpr auto NATIVE_NEWLINE = VSL_NATIVE_NEWLINE;

}  // namespace vsl

#endif  // VSL_OS_H
