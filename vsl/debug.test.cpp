#include "debug.h"

#include <vsl/types.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fmt/format.h>

#include <string>
#include <vector>

namespace vsl::debug::test
{

TEST(DebugTest, DebugAllocator)
{
    auto log = std::vector<std::string>{};
    auto expected_log = std::vector<std::string>{};

    auto handler = [&log](vsl::debug::DebugAllocatorOperation op, size_t total_size, size_t count, size_t size_of_type)
    {
        const auto op_mark = (op == vsl::debug::DebugAllocatorOperation::ALLOCATE) ? '+' : '-';
        const auto msg = fmt::format("({}) {} ({} of size={})", op_mark, total_size, count, size_of_type);
        log.push_back(msg);
    };

    // const auto alloc = vsl::debug::DebugAllocator<int>::create_default();
    const auto alloc = vsl::debug::DebugAllocator<int>{handler};

    {
        auto vec = std::vector<int, vsl::debug::DebugAllocator<int>>{alloc};

        vec.reserve(4);
        expected_log.push_back("(+) 16 (4 of size=4)");

        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        vec.push_back(4);

        vec.reserve(100);
        expected_log.push_back("(+) 400 (100 of size=4)");
        expected_log.push_back("(-) 16 (4 of size=4)");

        vec.resize(1);
        vec.clear();
    }
    expected_log.push_back("(-) 400 (100 of size=4)");

    EXPECT_EQ(log, expected_log);
}

}  // namespace vsl::debug::test
