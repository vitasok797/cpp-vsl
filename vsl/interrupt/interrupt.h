#ifndef VSL_INTERRUPT_INTERRUPT_H
#define VSL_INTERRUPT_INTERRUPT_H

#include <functional>

namespace vsl::interrupt
{

using HandlerFunc = std::function<void()>;

enum class SuppressCtrlC
{
    YES,
    NO,
};

inline auto set_handler(HandlerFunc handler) noexcept -> void;
inline auto start_handling(SuppressCtrlC suppress_ctrl_c = SuppressCtrlC::NO) noexcept -> void;
inline auto is_interrupted() noexcept -> bool;

}  // namespace vsl::interrupt

#include "interrupt_impl.h"

#endif  // VSL_INTERRUPT_INTERRUPT_H
