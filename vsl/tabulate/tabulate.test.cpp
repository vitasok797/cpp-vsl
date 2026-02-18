#include "tabulate.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fmt/format.h>
#include <tabulate/table.hpp>

#include <string>
#include <vector>

namespace vsl::tabulate::test
{

static auto header = {"ID", "Name", "Type"};
static auto empty_header = std::vector<std::string>{};
static auto item_to_cells = [](int x)
{
    return std::vector{
        fmt::format("{}", x),
        fmt::format("{}", x * 10),
        fmt::format("{}", x * 100),
    };
};

static auto get_table(int row_count, vsl::tabulate::HeaderType header_type) -> std::string
{
    auto table = ::tabulate::Table{};
    for (auto i = int{0}; i < row_count; ++i)
    {
        table.add_row({fmt::format("A{}", i), fmt::format("B{}", i)});
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

TEST(TabulateTest, CreateTable)
{
    auto items = {1, 2, 3, 4, 5};
    ASSERT_EQ(create_table(header, items, item_to_cells), "+----+------+------+\n"
                                                          "| ID | Name | Type |\n"
                                                          "+----+------+------+\n"
                                                          "| 1  | 10   | 100  |\n"
                                                          "| 2  | 20   | 200  |\n"
                                                          "| 3  | 30   | 300  |\n"
                                                          "| 4  | 40   | 400  |\n"
                                                          "| 5  | 50   | 500  |\n"
                                                          "+----+------+------+");
}

TEST(TabulateTest, CreateTableNoHeader)
{
    auto items = {1, 2, 3, 4, 5};
    ASSERT_EQ(create_table(empty_header, items, item_to_cells), "+---+----+-----+\n"
                                                                "| 1 | 10 | 100 |\n"
                                                                "| 2 | 20 | 200 |\n"
                                                                "| 3 | 30 | 300 |\n"
                                                                "| 4 | 40 | 400 |\n"
                                                                "| 5 | 50 | 500 |\n"
                                                                "+---+----+-----+");
}

TEST(TabulateTest, CreateTableOneRow)
{
    auto items = {1};
    ASSERT_EQ(create_table(header, items, item_to_cells), "+----+------+------+\n"
                                                          "| ID | Name | Type |\n"
                                                          "+----+------+------+\n"
                                                          "| 1  | 10   | 100  |\n"
                                                          "+----+------+------+");
}

TEST(TabulateTest, CreateTableOneRowNoHeader)
{
    auto items = {1};
    ASSERT_EQ(create_table(empty_header, items, item_to_cells), "+---+----+-----+\n"
                                                                "| 1 | 10 | 100 |\n"
                                                                "+---+----+-----+");
}

TEST(TabulateTest, CreateTableVariableCellCount)
{
    auto items = {1, 2, 3};
    static auto item_to_cells_variable = [](int x)
    {
        return std::vector(x, fmt::format("{}", x));  //
    };
    ASSERT_EQ(create_table(header, items, item_to_cells_variable), "+----+------+------+\n"
                                                                   "| ID | Name | Type |\n"
                                                                   "+----+------+------+\n"
                                                                   "| 1  |      |      |\n"
                                                                   "| 2  | 2    |      |\n"
                                                                   "| 3  | 3    | 3    |\n"
                                                                   "+----+------+------+");
}

}  // namespace vsl::tabulate::test
