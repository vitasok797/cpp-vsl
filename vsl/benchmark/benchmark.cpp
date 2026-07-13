#include <./nanobench.h>
#include <fmt/format.h>

#include <cstddef>
#include <immintrin.h>

// Reference:
// https://nanobench.ankerl.com/

using namespace ankerl::nanobench;

auto flush_cache(const void* ptr, size_t size) -> void
{
    constexpr auto CACHE_LINE_SIZE = size_t{64};
    const char* char_ptr = static_cast<const char*>(ptr);
    for (size_t i = 0; i < size; i += CACHE_LINE_SIZE)
    {
        _mm_clflush(char_ptr + i);
    }
    _mm_mfence();
}

// --------------------------------------------------------------------------------------------------------------------

// <code to test>

// --------------------------------------------------------------------------------------------------------------------

auto test(auto&& bench, const std::string& test_name /* test data args */) -> void
{
    bench.run(test_name + " / OPTION",
              [&]
              {
                  // flush_cache(&data, sizeof(data));

                  // <call test code>
                  // res =

                  // ankerl::nanobench::doNotOptimizeAway(res);
              });

    fmt::println("");
}

// --------------------------------------------------------------------------------------------------------------------

auto main() -> int
{
    auto bench = Bench();
    // bench.epochs(50);
    // bench.minEpochIterations(1000);
    // bench.warmup(100);
    // bench.relative(true);

    test(bench, "TEST" /* test data args */);
}
