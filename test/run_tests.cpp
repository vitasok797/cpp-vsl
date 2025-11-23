#include "run_tests.h"

auto main() -> int
{
    vsl::tabulate::test::test_tabulate();
    vsl::tcp::test::test_tcp();
    // vsl::interrupt::example::interrupt_example();
}
