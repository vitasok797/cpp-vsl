#include "tabulate.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <tabulate/table.hpp>

#include <format>
#include <string>

namespace vsl::tabulate::test
{

static auto get_table(int row_count, vsl::tabulate::HeaderType header_type) -> std::string
{
    auto table = ::tabulate::Table{};
    for (auto i = int{0}; i < row_count; ++i)
    {
        table.add_row({std::format("A{}", i), std::format("B{}", i)});
    }

    vsl::tabulate::hide_inner_borders(table, header_type);

    return table.str();
}

TEST(TabulateTest, HideInnerBordersWithHeader)
{
    ASSERT_EQ(get_table(4, vsl::tabulate::HeaderType::SEPARATED), "+----+----+\n"
                                                                  "| A0 | B0 |\n"
                                                                  "+----+----+\n"
                                                                  "| A1 | B1 |\n"
                                                                  "| A2 | B2 |\n"
                                                                  "| A3 | B3 |\n"
                                                                  "+----+----+");

    ASSERT_EQ(get_table(3, vsl::tabulate::HeaderType::SEPARATED), "+----+----+\n"
                                                                  "| A0 | B0 |\n"
                                                                  "+----+----+\n"
                                                                  "| A1 | B1 |\n"
                                                                  "| A2 | B2 |\n"
                                                                  "+----+----+");

    ASSERT_EQ(get_table(2, vsl::tabulate::HeaderType::SEPARATED), "+----+----+\n"
                                                                  "| A0 | B0 |\n"
                                                                  "+----+----+\n"
                                                                  "| A1 | B1 |\n"
                                                                  "+----+----+");

    ASSERT_EQ(get_table(1, vsl::tabulate::HeaderType::SEPARATED), "+----+----+\n"
                                                                  "| A0 | B0 |\n"
                                                                  "+----+----+");
}

TEST(TabulateTest, HideInnerBordersWithoutHeader)
{
    ASSERT_EQ(get_table(3, vsl::tabulate::HeaderType::NOT_SEPARATED), "+----+----+\n"
                                                                      "| A0 | B0 |\n"
                                                                      "| A1 | B1 |\n"
                                                                      "| A2 | B2 |\n"
                                                                      "+----+----+");

    ASSERT_EQ(get_table(2, vsl::tabulate::HeaderType::NOT_SEPARATED), "+----+----+\n"
                                                                      "| A0 | B0 |\n"
                                                                      "| A1 | B1 |\n"
                                                                      "+----+----+");

    ASSERT_EQ(get_table(1, vsl::tabulate::HeaderType::NOT_SEPARATED), "+----+----+\n"
                                                                      "| A0 | B0 |\n"
                                                                      "+----+----+");
}

}  // namespace vsl::tabulate::test
