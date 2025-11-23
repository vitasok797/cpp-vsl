#ifndef VSL_TABULATE_TABULATE_H
#define VSL_TABULATE_TABULATE_H

#include <tabulate/table.hpp>

namespace vsl::tabulate
{

enum class HeaderType
{
    SEPARATED,
    NOT_SEPARATED
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

}  // namespace vsl::tabulate

#endif  // VSL_TABULATE_TABULATE_H
