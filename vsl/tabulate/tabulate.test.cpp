#include "tabulate.h"

#include <tabulate/table.hpp>

#include <cassert>
#include <format>
#include <iostream>
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

static auto test_hide_inner_borders_with_header()
{
    assert(get_table(4, vsl::tabulate::HeaderType::SEPARATED)
           == "+----+----+\n"
              "| A0 | B0 |\n"
              "+----+----+\n"
              "| A1 | B1 |\n"
              "| A2 | B2 |\n"
              "| A3 | B3 |\n"
              "+----+----+");

    assert(get_table(3, vsl::tabulate::HeaderType::SEPARATED)
           == "+----+----+\n"
              "| A0 | B0 |\n"
              "+----+----+\n"
              "| A1 | B1 |\n"
              "| A2 | B2 |\n"
              "+----+----+");

    assert(get_table(2, vsl::tabulate::HeaderType::SEPARATED)
           == "+----+----+\n"
              "| A0 | B0 |\n"
              "+----+----+\n"
              "| A1 | B1 |\n"
              "+----+----+");

    assert(get_table(1, vsl::tabulate::HeaderType::SEPARATED)
           == "+----+----+\n"
              "| A0 | B0 |\n"
              "+----+----+");
}

static auto test_hide_inner_borders_without_header()
{
    assert(get_table(3, vsl::tabulate::HeaderType::NOT_SEPARATED)
           == "+----+----+\n"
              "| A0 | B0 |\n"
              "| A1 | B1 |\n"
              "| A2 | B2 |\n"
              "+----+----+");

    assert(get_table(2, vsl::tabulate::HeaderType::NOT_SEPARATED)
           == "+----+----+\n"
              "| A0 | B0 |\n"
              "| A1 | B1 |\n"
              "+----+----+");

    assert(get_table(1, vsl::tabulate::HeaderType::NOT_SEPARATED)
           == "+----+----+\n"
              "| A0 | B0 |\n"
              "+----+----+");
}

auto test_tabulate() -> void
{
    std::cout << "testing \"tabulate\": ";

    test_hide_inner_borders_with_header();
    test_hide_inner_borders_without_header();

    std::cout << "OK" << std::endl;
}

}  // namespace vsl::tabulate::test
