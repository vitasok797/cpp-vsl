#include "interrupt.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

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

auto main() -> int
{
    std::cout << "main thread[" << std::this_thread::get_id() << "] " << std::endl;

    vsl::interrupt::set_handler(interrupt_handler);

    // vsl::interrupt::start_handling();
    vsl::interrupt::start_handling(vsl::interrupt::SuppressCtrlC::YES);

    while (true)
    {
        if (vsl::interrupt::is_interrupted())
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
