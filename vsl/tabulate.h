#ifndef VSL_TABULATE_H
#define VSL_TABULATE_H

#include <vsl/concepts.h>
#include <vsl/util.h>

#include <tabulate/table.hpp>

#include <ranges>
#include <string>

namespace vsl::tabulate
{

using Table = ::tabulate::Table;
using TableRow = Table::Row_t;

enum class RowBorders
{
    ALL,
    HEADER,
    NONE,
};

inline auto format_row_borders(Table& table, RowBorders row_borders) -> void
{
    table.format().show_border_top();

    if (vsl::is_one_of(row_borders, {RowBorders::HEADER, RowBorders::NONE}))
    {
        table.format().hide_border_top();
        table[0].format().show_border_top();

        if (row_borders == RowBorders::HEADER && table.size() > 1)
        {
            table[1].format().show_border_top();
        }
    }
}

template<std::ranges::input_range Header, std::ranges::input_range Items>
auto create_table(const Header& header, const Items& items, auto item_to_row) -> std::string
    requires vsl::range_of_string_like<Header>
{
    auto table = Table{};

    if (header.size() > 0)
    {
        auto row = TableRow(header.begin(), header.end());
        table.add_row(row);
    }

    for (auto&& item : items)
    {
        table.add_row(item_to_row(item));
    }

    format_row_borders(table, (header.size() > 0) ? RowBorders::HEADER : RowBorders::NONE);

    return table.str();
}

}  // namespace vsl::tabulate

#endif  // VSL_TABULATE_H
