#ifndef VSL_TABULATE_TABULATE_H
#define VSL_TABULATE_TABULATE_H

#include <tabulate/table.hpp>

#include <ranges>
#include <string>

namespace vsl::tabulate
{

enum class HeaderType
{
    SEPARATED,
    NOT_SEPARATED,
};

inline auto hide_inner_borders(::tabulate::Table& table, HeaderType header_type) -> void
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
                  auto item_to_cells) -> std::string
{
    auto table = ::tabulate::Table{};
    auto row = ::tabulate::Table::Row_t{};
    auto header_type = HeaderType::NOT_SEPARATED;

    if (header.size() > 0)
    {
        row.assign(header.begin(), header.end());
        table.add_row(row);
        header_type = HeaderType::SEPARATED;
    }

    for (auto&& item : items)
    {
        auto cells = item_to_cells(item);
        row.assign(cells.begin(), cells.end());
        table.add_row(row);
    }

    hide_inner_borders(table, header_type);
    return table.str();
}

}  // namespace vsl::tabulate

#endif  // VSL_TABULATE_TABULATE_H
