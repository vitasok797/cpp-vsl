#include <./nanobench.h>

// Reference:
// https://nanobench.ankerl.com/

using namespace ankerl::nanobench;

auto main() -> int
{
    Bench()
        //.epochs(100)
        //.minEpochIterations(50)
        .run("TEST",
             [&]
             {
                 /* Your code here */
                 // ankerl::nanobench::doNotOptimizeAway(res);
             });
}
