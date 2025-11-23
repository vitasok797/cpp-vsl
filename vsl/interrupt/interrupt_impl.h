#ifndef VSL_INTERRUPT_INTERRUPT_IMPL_H
#define VSL_INTERRUPT_INTERRUPT_IMPL_H

#include <atomic>
#include <mutex>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#else
#include <csignal>
#include <pthread.h>
#endif

namespace vsl::interrupt
{

namespace detail
{

inline auto interrupted = std::atomic_bool{false};
inline auto handler_func = HandlerFunc{};
inline auto handler_mutex = std::mutex{};

inline auto take_handler() noexcept -> HandlerFunc
{
    auto _ = std::scoped_lock(handler_mutex);
    return std::move(handler_func);
}

#ifdef _WIN32

inline auto WINAPI invoke_handler(DWORD) noexcept -> BOOL
{
    interrupted = true;

    auto handler = take_handler();
    if (!handler) return false;

    handler();

    return true;
}

inline auto register_handler(SuppressCtrlC suppress_ctrl_c) noexcept -> void
{
    if (suppress_ctrl_c == SuppressCtrlC::YES)
    {
        SetConsoleCtrlHandler(NULL, TRUE);
    }
    SetConsoleCtrlHandler(invoke_handler, TRUE);
}

#else

inline auto signals_to_wait = sigset_t{};

inline auto invoke_handler() noexcept -> void
{
    interrupted = true;

    auto handler = take_handler();
    if (!handler) return;

    handler();
}

extern "C" inline auto wait_for_signal(void*) noexcept -> void*
{
    while (true)
    {
        auto signal = int{};
        auto rc = sigwait(&signals_to_wait, &signal);
        if (rc != EINTR) break;
    }

    invoke_handler();
    return nullptr;
}

inline auto register_handler(SuppressCtrlC suppress_ctrl_c) noexcept -> void
{
    auto signals = sigset_t{};
    sigemptyset(&signals);
    sigaddset(&signals, SIGHUP);
    sigaddset(&signals, SIGINT);
    sigaddset(&signals, SIGTERM);

    pthread_sigmask(SIG_BLOCK, &signals, nullptr);

    signals_to_wait = signals;
    if (suppress_ctrl_c == SuppressCtrlC::YES)
    {
        sigdelset(&signals_to_wait, SIGINT);
    }

    auto thread_id = pthread_t{};
    pthread_create(&thread_id, nullptr, wait_for_signal, nullptr);
}

#endif

}  // namespace detail

inline auto set_handler(HandlerFunc handler) noexcept -> void
{
    auto _ = std::scoped_lock(detail::handler_mutex);
    detail::handler_func = std::move(handler);
}

inline auto start_handling(SuppressCtrlC suppress_ctrl_c) noexcept -> void
{
    static auto started = false;
    if (!started)
    {
        detail::register_handler(suppress_ctrl_c);
        started = true;
    }
}

inline auto is_interrupted() noexcept -> bool
{
    return static_cast<bool>(detail::interrupted);
}

}  // namespace vsl::interrupt

#endif  // VSL_INTERRUPT_INTERRUPT_IMPL_H
