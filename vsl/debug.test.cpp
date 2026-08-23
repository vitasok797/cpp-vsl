#include "debug.h"

#include <vsl/types.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fmt/format.h>

#include <string>
#include <utility>
#include <vector>

using namespace testing;

namespace test
{

TEST(DebugTest, CopyMoveStatsNoOps)
{
    const auto stats = vsl::CopyMoveStats{};

    EXPECT_EQ(stats.copies, 0);
    EXPECT_EQ(stats.moves, 0);

    EXPECT_TRUE(stats.no_ops());
    EXPECT_FALSE(stats.only_copies());
    EXPECT_FALSE(stats.only_moves());
}

TEST(DebugTest, CopyMoveStatsOnlyCopies)
{
    const auto stats = vsl::CopyMoveStats{
        .copies = 2,
        .moves = 0,
    };

    EXPECT_FALSE(stats.no_ops());

    EXPECT_TRUE(stats.only_copies());
    EXPECT_FALSE(stats.only_copies(1));
    EXPECT_TRUE(stats.only_copies(2));

    EXPECT_FALSE(stats.only_moves());
}

TEST(DebugTest, CopyMoveStatsOnlyMoves)
{
    const auto stats = vsl::CopyMoveStats{
        .copies = 0,
        .moves = 2,
    };

    EXPECT_FALSE(stats.no_ops());

    EXPECT_FALSE(stats.only_copies());

    EXPECT_TRUE(stats.only_moves());
    EXPECT_FALSE(stats.only_moves(1));
    EXPECT_TRUE(stats.only_moves(2));
}

TEST(DebugTest, CopyMoveStatsMixedOperations)
{
    const auto stats = vsl::CopyMoveStats{
        .copies = 1,
        .moves = 1,
    };

    EXPECT_FALSE(stats.no_ops());

    EXPECT_FALSE(stats.only_copies());
    EXPECT_FALSE(stats.only_copies(1));

    EXPECT_FALSE(stats.only_moves());
    EXPECT_FALSE(stats.only_moves(1));
}

TEST(DebugTest, CopyMoveStatsNonPositiveCount)
{
    const auto stats = vsl::CopyMoveStats{};

    EXPECT_THROW(static_cast<void>(stats.only_copies(0)), vsl::AssertionError);
    EXPECT_THROW(static_cast<void>(stats.only_copies(-1)), vsl::AssertionError);

    EXPECT_THROW(static_cast<void>(stats.only_moves(0)), vsl::AssertionError);
    EXPECT_THROW(static_cast<void>(stats.only_moves(-1)), vsl::AssertionError);
}

TEST(DebugTest, CopyMoveStatsClear)
{
    auto stats = vsl::CopyMoveStats{
        .copies = 2,
        .moves = 1,
    };

    EXPECT_FALSE(stats.no_ops());
    stats.clear();
    EXPECT_TRUE(stats.no_ops());
}

TEST(DebugTest, CopyMoveProbeCopy)
{
    auto stats = vsl::CopyMoveStats{};
    auto source = vsl::CopyMoveProbe{stats};

    EXPECT_TRUE(stats.no_ops());

    auto copy = source;

    EXPECT_TRUE(stats.only_copies(1));

    [[maybe_unused]] auto copy2 = copy;

    EXPECT_TRUE(stats.only_copies(2));
}

TEST(DebugTest, CopyMoveProbeMove)
{
    auto stats = vsl::CopyMoveStats{};
    auto source = vsl::CopyMoveProbe{stats};

    EXPECT_TRUE(stats.no_ops());

    auto moved = std::move(source);

    EXPECT_TRUE(stats.only_moves(1));

    [[maybe_unused]] auto moved2 = std::move(moved);

    EXPECT_TRUE(stats.only_moves(2));
}

TEST(DebugTest, CopyMoveProbeCopyMove)
{
    auto stats = vsl::CopyMoveStats{};
    auto source = vsl::CopyMoveProbe{stats};

    EXPECT_TRUE(stats.no_ops());

    auto copy = source;

    EXPECT_TRUE(stats.only_copies(1));

    [[maybe_unused]] auto moved = std::move(copy);

    EXPECT_EQ(stats.copies, 1);
    EXPECT_EQ(stats.moves, 1);

    EXPECT_FALSE(stats.only_copies());
    EXPECT_FALSE(stats.only_copies(1));

    EXPECT_FALSE(stats.only_moves());
    EXPECT_FALSE(stats.only_moves(1));
}

TEST(DebugTest, CopyMoveProbeCopyAssign)
{
    auto source_stats = vsl::CopyMoveStats{};
    auto target_stats = vsl::CopyMoveStats{};

    auto source = vsl::CopyMoveProbe{source_stats};
    auto target = vsl::CopyMoveProbe{target_stats};

    target = source;  // target now tracks source_stats

    EXPECT_TRUE(source_stats.only_copies(1));
    EXPECT_TRUE(target_stats.no_ops());  // target_stats is no longer bound to target

    [[maybe_unused]] auto target_copy = target;

    EXPECT_TRUE(source_stats.only_copies(2));
    EXPECT_TRUE(target_stats.no_ops());  // target_stats is no longer bound to target
}

TEST(DebugTest, CopyMoveProbeMoveAssign)
{
    auto source_stats = vsl::CopyMoveStats{};
    auto target_stats = vsl::CopyMoveStats{};

    auto source = vsl::CopyMoveProbe{source_stats};
    auto target = vsl::CopyMoveProbe{target_stats};

    target = std::move(source);  // target now tracks source_stats

    EXPECT_TRUE(source_stats.only_moves(1));
    EXPECT_TRUE(target_stats.no_ops());  // target_stats is no longer bound to target

    [[maybe_unused]] auto target_moved = std::move(target);

    EXPECT_TRUE(source_stats.only_moves(2));
    EXPECT_TRUE(target_stats.no_ops());  // target_stats is no longer bound to target
}

TEST(DebugTest, CopyMoveProbeCheckOverloads)
{
    struct Subject
    {
        explicit Subject(const vsl::CopyMoveProbe& value)
            : value_{value}
        {}

        explicit Subject(vsl::CopyMoveProbe&& value)
            : value_{std::move(value)}
        {}

        vsl::CopyMoveProbe value_;
    };

    {
        auto stats = vsl::CopyMoveStats{};
        auto value = vsl::CopyMoveProbe{stats};

        [[maybe_unused]] auto subject = Subject{value};

        EXPECT_TRUE(stats.only_copies(1));
    }
    {
        auto stats = vsl::CopyMoveStats{};
        auto value = vsl::CopyMoveProbe{stats};

        [[maybe_unused]] auto subject = Subject{std::move(value)};

        EXPECT_TRUE(stats.only_moves(1));
    }
}

TEST(DebugTest, CopyMoveProbeCheckForwarding)
{
    auto make_subject = []<typename T>(T&& value) { return vsl::CopyMoveProbe{std::forward<T>(value)}; };

    {
        auto stats = vsl::CopyMoveStats{};
        auto value = vsl::CopyMoveProbe{stats};

        [[maybe_unused]] auto subject = make_subject(value);

        EXPECT_TRUE(stats.only_copies(1));
    }
    {
        auto stats = vsl::CopyMoveStats{};
        auto value = vsl::CopyMoveProbe{stats};

        [[maybe_unused]] auto subject = make_subject(std::move(value));

        EXPECT_TRUE(stats.only_moves(1));
    }
}

TEST(DebugTest, DebugAllocator)
{
    auto log = std::vector<std::string>{};
    auto expected_log = std::vector<std::string>{};

    auto handler = [&log](vsl::DebugAllocatorOperation op, size_t total_size, size_t count, size_t size_of_type)
    {
        const auto op_mark = (op == vsl::DebugAllocatorOperation::ALLOCATE) ? '+' : '-';
        const auto msg = fmt::format("({}) {} ({} of size={})", op_mark, total_size, count, size_of_type);
        log.push_back(msg);
    };

    // const auto alloc = vsl::DebugAllocator<int>::create_default();
    const auto alloc = vsl::DebugAllocator<int>{handler};

    {
        auto vec = std::vector<int, vsl::DebugAllocator<int>>{alloc};

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

}  // namespace test
