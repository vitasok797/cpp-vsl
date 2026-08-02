#include "table.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fmt/format.h>

#include <initializer_list>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace test
{

static auto fill_simple_table(auto&& table) -> void
{
    table.write_ln("A", "B");
    table.write_ln("1", "2");
}

static auto fill_simple_table_u8(auto&& table) -> void
{
    table.write_ln("A", "B");
    table.write_ln("юникод", "тест");
}

TEST(TableTest, Create)
{
    {
        auto table = vsl::TableAscii{};
        fill_simple_table(table);
        EXPECT_EQ(table.to_string(), "+---+---+\n"
                                     "| A | B |\n"
                                     "| 1 | 2 |\n"
                                     "+---+---+");
    }
    {
        auto table = vsl::Table{};
        fill_simple_table_u8(table);
        EXPECT_EQ(table.to_string(), "+--------+------+\n"
                                     "| A      | B    |\n"
                                     "| юникод | тест |\n"
                                     "+--------+------+");
    }
}

TEST(TableTest, CreateWithStyle)
{
    {
        auto table = vsl::TableAscii{vsl::TableBorderStyle::SOLID};
        fill_simple_table(table);
        EXPECT_EQ(table.to_string(), "┌───┬───┐\n"
                                     "│ A │ B │\n"
                                     "│ 1 │ 2 │\n"
                                     "└───┴───┘");
    }
    {
        auto table = vsl::Table{vsl::TableBorderStyle::DOUBLE};
        fill_simple_table_u8(table);
        EXPECT_EQ(table.to_string(), "╔════════╦══════╗\n"
                                     "║ A      ║ B    ║\n"
                                     "║ юникод ║ тест ║\n"
                                     "╚════════╩══════╝");
    }
}

TEST(TableTest, CreateFromRange)
{
    const auto header = {"A", "B"};
    const auto items = {1, 2, 3};
    auto item_to_row = [](auto& table, int x)
    {
        table << x;
        table.write(fmt::format("x={}", x));
    };

    {
        auto table = vsl::TableAscii{header, items, item_to_row};
        EXPECT_EQ(table.to_string(), "+---+-----+\n"
                                     "| A | B   |\n"
                                     "+---+-----+\n"
                                     "| 1 | x=1 |\n"
                                     "| 2 | x=2 |\n"
                                     "| 3 | x=3 |\n"
                                     "+---+-----+");
    }
    {
        auto table = vsl::Table{header, items, item_to_row};
        EXPECT_EQ(table.to_string(), "+---+-----+\n"
                                     "| A | B   |\n"
                                     "+---+-----+\n"
                                     "| 1 | x=1 |\n"
                                     "| 2 | x=2 |\n"
                                     "| 3 | x=3 |\n"
                                     "+---+-----+");
    }
    {
        auto table = vsl::TableAscii{header, items, item_to_row, vsl::TableBorderStyle::SOLID};
        EXPECT_EQ(table.to_string(), "┌───┬─────┐\n"
                                     "│ A │ B   │\n"
                                     "├───┼─────┤\n"
                                     "│ 1 │ x=1 │\n"
                                     "│ 2 │ x=2 │\n"
                                     "│ 3 │ x=3 │\n"
                                     "└───┴─────┘");
    }
    {
        auto table = vsl::Table{header, items, item_to_row, vsl::TableBorderStyle::DOUBLE};
        EXPECT_EQ(table.to_string(), "╔═══╦═════╗\n"
                                     "║ A ║ B   ║\n"
                                     "╠═══╬═════╣\n"
                                     "║ 1 ║ x=1 ║\n"
                                     "║ 2 ║ x=2 ║\n"
                                     "║ 3 ║ x=3 ║\n"
                                     "╚═══╩═════╝");
    }
    {
        // Empty header
        auto table = vsl::TableAscii{std::views::empty<std::string>, items, item_to_row};
        EXPECT_EQ(table.to_string(), "+---+-----+\n"
                                     "| 1 | x=1 |\n"
                                     "| 2 | x=2 |\n"
                                     "| 3 | x=3 |\n"
                                     "+---+-----+");
    }
    {
        // Empty items
        auto table = vsl::TableAscii{header, std::views::empty<int>, item_to_row};
        EXPECT_EQ(table.to_string(), "+---+---+\n"
                                     "| A | B |\n"
                                     "+---+---+");
    }
}

TEST(TableTest, EmptyTable)
{
    auto table = vsl::Table{};
    EXPECT_EQ(table.to_string(), "");
}

TEST(TableTest, SingleRow)
{
    auto table = vsl::Table{};
    table.write_ln("A", "B");
    EXPECT_EQ(table.to_string(), "+---+---+\n"
                                 "| A | B |\n"
                                 "+---+---+");
}

TEST(TableTest, SetBorderStyle)
{
    auto table = vsl::Table{};
    fill_simple_table(table);

    table.set_border_style(vsl::TableBorderStyle::SOLID_ROUND);

    EXPECT_EQ(table.to_string(), "╭───┬───╮\n"
                                 "│ A │ B │\n"
                                 "│ 1 │ 2 │\n"
                                 "╰───┴───╯");
}

TEST(TableTest, FillStream)
{
    auto table = vsl::Table{};

    table << vsl::start_table_header_row << "A" << "B" << vsl::end_table_row;

    table << "1" << "2" << vsl::end_table_row;
    table << std::string{"3"} << std::string_view{"4"} << vsl::end_table_row;

    table << vsl::add_table_separator;

    table << 5 << 6 << vsl::end_table_row;      // int to string conversion
    table << "Э" << "Ы" << vsl::end_table_row;  // utf-8

    EXPECT_EQ(table.to_string(), "+---+---+\n"
                                 "| A | B |\n"
                                 "+---+---+\n"
                                 "| 1 | 2 |\n"
                                 "| 3 | 4 |\n"
                                 "+---+---+\n"
                                 "| 5 | 6 |\n"
                                 "| Э | Ы |\n"
                                 "+---+---+");
}

TEST(TableTest, FillWrite)
{
    auto table = vsl::Table{};

    table.start_header_row();
    table.write_ln("A", "B");

    table.write("1", 2);  // mixed arg types
    table.end_row();

    table.write(std::string{"3"}, std::string{"4"});
    table.end_row();

    table.write(std::string_view{"5"}, std::string_view{"6"});
    table.end_row();

    table.add_separator();

    table.write_ln(std::string{"7"}, std::string{"8"});
    table.write_ln(std::string_view{"9"}, std::string_view{"0"});
    table.write_ln("Э", "Ы");  // utf-8

    EXPECT_EQ(table.to_string(), "+---+---+\n"
                                 "| A | B |\n"
                                 "+---+---+\n"
                                 "| 1 | 2 |\n"
                                 "| 3 | 4 |\n"
                                 "| 5 | 6 |\n"
                                 "+---+---+\n"
                                 "| 7 | 8 |\n"
                                 "| 9 | 0 |\n"
                                 "| Э | Ы |\n"
                                 "+---+---+");
}

TEST(TableTest, FillRangeWrite)
{
    const auto header = std::initializer_list<const char*>{"A", "B"};
    const auto line1 = std::initializer_list<std::string>{"1", "2"};
    const auto line2 = std::initializer_list<std::string_view>{"3", "4"};
    const auto line3 = std::vector{5, 6};      // int to string conversion
    const auto line4 = std::vector{"Э", "Ы"};  // utf-8

    auto table = vsl::Table{};

    table.start_header_row();
    table.write_ln(header);

    table.write(line1);
    table.end_row();

    table.write(line2);
    table.end_row();

    table.add_separator();

    table.write_ln(line3);
    table.write_ln(line4);

    EXPECT_EQ(table.to_string(), "+---+---+\n"
                                 "| A | B |\n"
                                 "+---+---+\n"
                                 "| 1 | 2 |\n"
                                 "| 3 | 4 |\n"
                                 "+---+---+\n"
                                 "| 5 | 6 |\n"
                                 "| Э | Ы |\n"
                                 "+---+---+");
}

TEST(TableTest, WriteHeader)
{
    {
        auto table = vsl::Table{};

        table.write_header_ln("A", 333);  // mixed arg types

        table.write_ln("1", "2");
        table.write_ln("3", "4");

        EXPECT_EQ(table.to_string(), "+---+-----+\n"
                                     "| A | 333 |\n"
                                     "+---+-----+\n"
                                     "| 1 | 2   |\n"
                                     "| 3 | 4   |\n"
                                     "+---+-----+");
    }
    {
        auto table = vsl::Table{};

        const auto header = {"A", "B"};
        table.write_header_ln(header);

        table.write_ln("1", "2");
        table.write_ln("3", "4");

        EXPECT_EQ(table.to_string(), "+---+---+\n"
                                     "| A | B |\n"
                                     "+---+---+\n"
                                     "| 1 | 2 |\n"
                                     "| 3 | 4 |\n"
                                     "+---+---+");
    }
}

TEST(TableTest, WriteSingleString)
{
    auto table = vsl::Table{};

    {
        const auto const_lvalue = std::string{"AB"};
        table.write_header_ln(const_lvalue);
    }
    {
        const auto const_lvalue = "11";
        table.write(const_lvalue);

        auto lvalue = "12";
        table.write(lvalue);

        table.write("13");

        table.end_row();
    }
    {
        const auto const_lvalue = std::string{"21"};
        table.write(const_lvalue);

        auto lvalue = std::string{"22"};
        table.write(lvalue);

        table.write(std::string{"23"});

        table.end_row();
    }
    {
        const auto const_lvalue = std::string_view{"31"};
        table.write(const_lvalue);

        auto lvalue = std::string_view{"32"};
        table.write(lvalue);

        table.write(std::string_view{"33"});

        table.end_row();
    }
    {
        const auto const_lvalue = "41";
        table.write_ln(const_lvalue);

        auto lvalue = "42";
        table.write_ln(lvalue);

        table.write_ln("43");
    }
    {
        const auto const_lvalue = std::string{"51"};
        table.write_ln(const_lvalue);

        auto lvalue = std::string{"52"};
        table.write_ln(lvalue);

        table.write_ln(std::string{"53"});
    }
    {
        const auto const_lvalue = std::string_view{"61"};
        table.write_ln(const_lvalue);

        auto lvalue = std::string_view{"62"};
        table.write_ln(lvalue);

        table.write_ln(std::string_view{"63"});
    }

    EXPECT_EQ(table.to_string(), "+----+----+----+\n"
                                 "| AB |    |    |\n"
                                 "+----+----+----+\n"
                                 "| 11 | 12 | 13 |\n"
                                 "| 21 | 22 | 23 |\n"
                                 "| 31 | 32 | 33 |\n"
                                 "| 41 |    |    |\n"
                                 "| 42 |    |    |\n"
                                 "| 43 |    |    |\n"
                                 "| 51 |    |    |\n"
                                 "| 52 |    |    |\n"
                                 "| 53 |    |    |\n"
                                 "| 61 |    |    |\n"
                                 "| 62 |    |    |\n"
                                 "| 63 |    |    |\n"
                                 "+----+----+----+");
}

TEST(TableTest, AddSeparator)
{
    auto table = vsl::Table{};

    table.write_ln(1, 2);
    table.write_ln(3, 4);

    table.write(5, 6);
    // table.end_row()
    table.add_separator();  // NOTE: Calling add_separator() does not imply an implicit call to end_row().
                            // Calling add_separator() inserts a separator before the "current" row
                            // (between rows "3, 4" and "5, 6")

    table.write_ln(7, 8);
    table.write_ln(9, 0);

    EXPECT_EQ(table.to_string(), "+---+---+---+---+\n"
                                 "| 1 | 2 |   |   |\n"
                                 "| 3 | 4 |   |   |\n"
                                 "+---+---+---+---+\n"
                                 "| 5 | 6 | 7 | 8 |\n"
                                 "| 9 | 0 |   |   |\n"
                                 "+---+---+---+---+");
}

TEST(TableTest, MoveCtor)
{
    {
        auto table = vsl::Table{};
        fill_simple_table(table);

        auto table2 = std::move(table);

        EXPECT_EQ(table.to_string(), "");
        EXPECT_EQ(table2.to_string(), "+---+---+\n"
                                      "| A | B |\n"
                                      "| 1 | 2 |\n"
                                      "+---+---+");
    }
    {
        auto table = vsl::TableAscii{};
        fill_simple_table(table);

        auto table2 = std::move(table);

        EXPECT_EQ(table.to_string(), "");
        EXPECT_EQ(table2.to_string(), "+---+---+\n"
                                      "| A | B |\n"
                                      "| 1 | 2 |\n"
                                      "+---+---+");
    }
}

TEST(TableTest, MoveAssignment)
{
    {
        auto table = vsl::Table{};
        fill_simple_table(table);

        auto table2 = vsl::Table{};
        table2 = std::move(table);

        EXPECT_EQ(table.to_string(), "");
        EXPECT_EQ(table2.to_string(), "+---+---+\n"
                                      "| A | B |\n"
                                      "| 1 | 2 |\n"
                                      "+---+---+");
    }
    {
        auto table = vsl::TableAscii{};
        fill_simple_table(table);

        auto table2 = vsl::TableAscii{};
        table2 = std::move(table);

        EXPECT_EQ(table.to_string(), "");
        EXPECT_EQ(table2.to_string(), "+---+---+\n"
                                      "| A | B |\n"
                                      "| 1 | 2 |\n"
                                      "+---+---+");
    }
}

}  // namespace test
