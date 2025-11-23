#include "interrupt.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

namespace vsl::interrupt::example
{

static auto interrupt_handler() noexcept -> void
{
#ifdef _WIN32
    auto platform_name = "Windows";
#else
    auto platform_name = "Linux";
#endif

    std::cout << platform_name << " handler:"                     //
              << " thread[" << std::this_thread::get_id() << "]"  //
              << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << platform_name << " handler: exit" << std::endl;

    std::exit(1);
}

auto interrupt_example() -> void
{
    std::cout << "main thread[" << std::this_thread::get_id() << "] " << std::endl;

    set_handler(interrupt_handler);

    // start_handling();
    start_handling(SuppressCtrlC::YES);

    while (true)
    {
        if (is_interrupted())
        {
            std::cout << "interrupted" << std::endl;
        }
        else
        {
            std::cout << "ok" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

}  // namespace vsl::interrupt::example
