#include <./nanobench.h>

// Reference:
// https://nanobench.ankerl.com/

using namespace ankerl::nanobench;

auto main() -> int
{
    auto bench = Bench()
        //.epochs(50)
        //.minEpochIterations(1000)
        //.relative(true)
        ;

    bench.run("NAME",
              [&]
              {
                  /* Your code here */
                  // ankerl::nanobench::doNotOptimizeAway(res);
              });
}
