#ifndef VSL_TABLE_H
#define VSL_TABLE_H

#include <vsl/concepts.h>

#include <./fort.hpp>

#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>

// Reference:
// https://github.com/seleznevae/libfort
// https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/Tutorial.md
// https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/CPP_API/Border-styles.md

namespace vsl
{

enum class TableBorderStyle
{
    // ASCII border symbols:
    BASIC,
    BASIC2,
    SIMPLE,
    PLAIN,
    EMPTY,

    // UTF-8 border symbols:
    SOLID,
    SOLID_ROUND,
    DOUBLE,
    DOUBLE2,
    BOLD,
    BOLD2,
};

inline const auto start_table_header_row = fort::header;
inline const auto end_table_row = fort::endr;
inline const auto add_table_separator = fort::separator;

namespace detail
{

template<typename BaseTable>
class TableExtension : public BaseTable
{
  public:
    TableExtension() = default;

    explicit TableExtension(TableBorderStyle style)
    {
        set_border_style(style);
    }

    template<std::ranges::input_range Header, std::ranges::input_range Items, typename Func>
    explicit TableExtension(const Header& header,
                            const Items& items,
                            Func item_to_row,
                            TableBorderStyle style = vsl::TableBorderStyle::BASIC)
    {
        if (!std::ranges::empty(header))
        {
            write_header_ln(header);
        }

        for (const auto& item : items)
        {
            item_to_row(*this, item);
            end_row();
        }

        set_border_style(style);
    }

    auto set_border_style(TableBorderStyle style) -> void
    {
        const struct ft_border_style* ft_style = nullptr;

        switch (style)
        {
        case TableBorderStyle::BASIC:
            ft_style = FT_BASIC_STYLE;
            break;
        case TableBorderStyle::BASIC2:
            ft_style = FT_BASIC2_STYLE;
            break;
        case TableBorderStyle::SIMPLE:
            ft_style = FT_SIMPLE_STYLE;
            break;
        case TableBorderStyle::PLAIN:
            ft_style = FT_PLAIN_STYLE;
            break;
        case TableBorderStyle::EMPTY:
            ft_style = FT_EMPTY_STYLE;
            break;
        case TableBorderStyle::SOLID:
            ft_style = FT_SOLID_STYLE;
            break;
        case TableBorderStyle::SOLID_ROUND:
            ft_style = FT_SOLID_ROUND_STYLE;
            break;
        case TableBorderStyle::DOUBLE:
            ft_style = FT_DOUBLE_STYLE;
            break;
        case TableBorderStyle::DOUBLE2:
            ft_style = FT_DOUBLE2_STYLE;
            break;
        case TableBorderStyle::BOLD:
            ft_style = FT_BOLD_STYLE;
            break;
        case TableBorderStyle::BOLD2:
            ft_style = FT_BOLD2_STYLE;
            break;
        }

        if (ft_style)
        {
            static_cast<BaseTable*>(this)->set_border_style(ft_style);
        }
    }

    auto start_header_row() -> void
    {
        *this << fort::header;
    }

    auto end_row() -> void
    {
        *this << fort::endr;
    }

    auto add_separator() -> void
    {
        *this << fort::separator;
    }

    template<std::ranges::input_range R>
        requires(!vsl::character<std::ranges::range_value_t<R>>)
    auto write(const R& rng) -> void
    {
        for (const auto& item : rng)
        {
            *this << item;
        }
    }

    template<typename... Args>
    auto write(Args&&... args) -> void
    {
        ((*this << std::forward<Args>(args)), ...);
    }

    template<std::ranges::input_range R>
        requires(!vsl::character<std::ranges::range_value_t<R>>)
    auto write_ln(const R& rng) -> void
    {
        write(rng);
        end_row();
    }

    template<typename... Args>
    auto write_ln(Args&&... args) -> void
    {
        write(std::forward<Args>(args)...);
        end_row();
    }

    template<std::ranges::input_range R>
        requires(!vsl::character<std::ranges::range_value_t<R>>)
    auto write_header_ln(const R& rng) -> void
    {
        start_header_row();
        write_ln(rng);
    }

    template<typename... Args>
    auto write_header_ln(Args&&... args) -> void
    {
        start_header_row();
        write_ln(std::forward<Args>(args)...);
    }

    auto to_string() const -> std::string
    {
        const auto* str = this->c_str();
        if (!str)
        {
            throw std::runtime_error("Error during table to string conversion");
        }

        auto table_str = std::string{str};
        if (!table_str.empty())
        {
            table_str.pop_back();
        }
        return table_str;
    }
};

}  // namespace detail

class Table : public detail::TableExtension<fort::utf8_table>
{
    using detail::TableExtension<fort::utf8_table>::TableExtension;
};

class TableAscii : public detail::TableExtension<fort::char_table>
{
    using detail::TableExtension<fort::char_table>::TableExtension;
};

}  // namespace vsl

#endif  // VSL_TABLE_H
