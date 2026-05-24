#ifndef VSL_DEBUG_H
#define VSL_DEBUG_H

#include <vsl/types.h>

#include <fmt/format.h>

#include <atomic>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <syncstream>
#include <type_traits>
#include <utility>

#define VSL_IS_VALUE(x) (std::is_reference_v<decltype(x)> ? "" : "VAL")
#define VSL_IS_REF(x) (std::is_lvalue_reference_v<decltype(x)> ? "REF" : "")
#define VSL_IS_RVALUE_REF(x) (std::is_rvalue_reference_v<decltype(x)> ? "RVAL_REF" : "")

namespace vsl::debug
{

inline auto get_thread_unique_num() noexcept -> int
{
    static auto next_thread_num = std::atomic<int>{0};
    static thread_local auto thread_num = next_thread_num.fetch_add(1);
    return thread_num;
}

template<typename... Values>
void println_sync(const Values&... values)
{
    auto os = std::osyncstream{std::cout};
    os << "[" << get_thread_unique_num() << "]";
    ((os << " " << values), ...);
    os << std::endl;
}

struct WatcherOptions
{
    bool print_mark{true};
    bool print_thread{true};
    bool print_identity{true};
    bool print_other{true};
    bool print_ctor{true};
    bool print_ctor_copy{true};
    bool print_ctor_move{true};
    bool print_assign_copy{true};
    bool print_assign_move{true};
    bool print_destructor{true};
    bool print_full_operation_category{true};
};

template<WatcherOptions options = {}>
class WatcherBase final
{
  public:
    WatcherBase() noexcept
        : index_{++counter_}
    {
        if (!options.print_ctor) return;
        print_event("constructed");
    }

    WatcherBase(int) noexcept
        : index_{++counter_}
    {
        if (!options.print_ctor) return;
        print_event("constructed ctor1");
    }

    WatcherBase(int, int) noexcept
        : index_{++counter_}
    {
        if (!options.print_ctor) return;
        print_event("constructed ctor2");
    }

    WatcherBase(const std::string& marker) noexcept
        : marker_{marker}
    {
        if (!options.print_ctor) return;
        print_event("constructed");
    }

    WatcherBase(const WatcherBase& other) noexcept
        : index_{++counter_},
          marker_{other.marker_}
    {
        if (options.print_ctor_copy)
        {
            auto message = options.print_full_operation_category ? "COPY constructed" : "COPY";
            print_event(message, &other);
        }
    }

    WatcherBase(WatcherBase&& other) noexcept
        : index_{++counter_},
          marker_{other.marker_}
    {
        if (options.print_ctor_move)
        {
            auto message = options.print_full_operation_category ? "move constructed" : "move";
            print_event(message, &other);
        }

        other.moved_ = true;
    }

    WatcherBase& operator=(const WatcherBase& other) noexcept
    {
        if (options.print_assign_copy)
        {
            auto message = options.print_full_operation_category ? "COPY assigned" : "COPY=";
            print_event(message, &other);
        }

        moved_ = false;
        return *this;
    }

    WatcherBase& operator=(WatcherBase&& other) noexcept
    {
        if (options.print_assign_move)
        {
            auto message = options.print_full_operation_category ? "move assigned" : "move=";
            print_event(message, &other);
        }

        moved_ = false;
        other.moved_ = true;
        return *this;
    }

    ~WatcherBase() noexcept
    {
        if (!options.print_destructor) return;
        print_event("destroyed");
    }

  private:
    auto print_event(std::string_view event_desc, const WatcherBase* other = nullptr) const -> void
    {
        auto os = std::osyncstream{std::cout};

        if (options.print_mark)
        {
            os << ">>> ";
        }

        if (options.print_thread)
        {
            os << "[" << get_thread_unique_num() << "] ";
        }

        if (options.print_identity)
        {
            output_identity(os);
            os << " ";
        }

        os << event_desc;

        if (other && options.print_other)
        {
            os << " from ";
            other->output_identity(os);
        }

        os << std::endl;
    }

    auto output_identity(std::osyncstream& os) const -> void
    {
        os << "(";
        if (marker_)
        {
            os << "\"" << *marker_ << "\"";
        }
        else
        {
            os << index_;
        }
        if (moved_) os << ", moved";
        os << ")";
    }

    static inline std::atomic<size_t> counter_{0};
    size_t index_{0};
    std::optional<std::string> marker_{};
    bool moved_{false};
};

namespace detail
{

inline constexpr auto watcher_config = WatcherOptions{
    .print_thread = false,
};

inline constexpr auto copy_watcher_config = WatcherOptions{
    .print_thread = false,
    .print_identity = false,
    .print_other = false,
    .print_ctor = false,
    .print_ctor_move = false,
    .print_assign_move = false,
    .print_destructor = false,
    .print_full_operation_category = false,
};

inline constexpr auto ctor_watcher_config = WatcherOptions{
    .print_thread = false,
    .print_identity = false,
    .print_other = false,
    .print_assign_copy = false,
    .print_assign_move = false,
    .print_destructor = false,
    .print_full_operation_category = false,
};

}  // namespace detail

using Watcher = WatcherBase<detail::watcher_config>;
using CopyWatcher = WatcherBase<detail::copy_watcher_config>;
using CtorWatcher = WatcherBase<detail::ctor_watcher_config>;
using ThreadWatcher = WatcherBase<>;

enum class DebugAllocatorOperation
{
    ALLOCATE,
    DEALLOCATE,
};

template<typename T>
class DebugAllocator
{
  public:
    using value_type = T;
    using Handler =
        std::function<void(DebugAllocatorOperation op, size_t total_size, size_t count, size_t size_of_type)>;

    static auto create_default() -> DebugAllocator
    {
        return DebugAllocator<T>{DebugAllocator<T>::default_handler};
    }

    DebugAllocator() = default;

    DebugAllocator(Handler handler) noexcept
        : handler_{std::move(handler)}
    {}

    template<typename U>
    constexpr DebugAllocator(const DebugAllocator<U>&) noexcept
    {}

    T* allocate(size_t count)
    {
        if (handler_)
        {
            const auto size_of_type = sizeof(value_type);
            const auto total_size = count * size_of_type;
            handler_(DebugAllocatorOperation::ALLOCATE, total_size, count, size_of_type);
        }
        return static_cast<T*>(::operator new(count * sizeof(T)));
    }

    void deallocate(T* p, size_t count) noexcept
    {
        if (handler_)
        {
            const auto size_of_type = sizeof(value_type);
            const auto total_size = count * size_of_type;
            handler_(DebugAllocatorOperation::DEALLOCATE, total_size, count, size_of_type);
        }
        ::operator delete(p);
    }

    friend bool operator==(const DebugAllocator&, const DebugAllocator&)
    {
        return true;
    }

    friend bool operator!=(const DebugAllocator&, const DebugAllocator&)
    {
        return false;
    }

  private:
    static auto default_handler(DebugAllocatorOperation op, size_t total_size, size_t count, size_t size_of_type)
        -> void
    {
        const auto op_mark = op == DebugAllocatorOperation::ALLOCATE ? '+' : '-';
        const auto msg = fmt::format("({}) {} ({} of size={})", op_mark, total_size, count, size_of_type);
        std::clog << msg << std::endl;
    }

    Handler handler_{};
};

}  // namespace vsl::debug

#endif  // VSL_DEBUG_H
