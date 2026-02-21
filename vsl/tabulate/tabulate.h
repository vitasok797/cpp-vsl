#ifndef VSL_TABULATE_TABULATE_H
#define VSL_TABULATE_TABULATE_H

#include <tabulate/table.hpp>

#include <ranges>
#include <string>

namespace vsl::tabulate
{

using Table = ::tabulate::Table;
using TableRow = Table::Row_t;

enum class HeaderType
{
    SEPARATED,
    NOT_SEPARATED,
};

inline auto hide_inner_borders(Table& table, HeaderType header_type) -> void
{
    table.format().hide_border_top();
    table[0].format().show_border_top();

    if (table.size() > 1 && header_type == HeaderType::SEPARATED)
    {
        table[1].format().show_border_top();
    }
}

auto create_table(const std::ranges::input_range auto& header,
                  const std::ranges::input_range auto& items,
                  auto item_to_row) -> std::string
{
    auto table = Table{};
    auto header_type = HeaderType::NOT_SEPARATED;

    if (header.size() > 0)
    {
        auto row = TableRow(header.begin(), header.end());
        table.add_row(row);
        header_type = HeaderType::SEPARATED;
    }

    for (auto&& item : items)
    {
        table.add_row(item_to_row(item));
    }

    hide_inner_borders(table, header_type);
    return table.str();
}

}  // namespace vsl::tabulate

#endif  // VSL_TABULATE_TABULATE_H
