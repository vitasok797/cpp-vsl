#include "csv.h"

#include <vsl/types.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <stdexcept>
#include <string>

using namespace testing;

namespace test
{

static constexpr auto TEXT_LF = "col1;col2\nval1;001\n  \nval2;  002\n\nval3\nval4;004;xxx\nval5;005\n";
static constexpr auto TEXT_CRLF =
    "col1;col2\r\nval1;001\r\n  \r\nval2;  002\r\n\r\nval3\r\nval4;004;xxx\r\nval5;005\r\n";
static constexpr auto TEXT_LF_BOM = "\xEF\xBB\xBF"
                                    "col1;col2\nval1;001\n  \nval2;  002\n\nval3\nval4;004;xxx\nval5;005\n";

static auto get_default_format() -> vsl::CsvFormat
{
    auto format = vsl::CsvFormat{};
    format.threading(false);
    format.delimiter({';', ','});
    format.trim({' ', '\t'});
    return format;
}

static auto process_text(const auto& format, vsl::cstring text) -> std::string
{
    auto ss = std::stringstream(text);
    auto csv = vsl::CsvReader{ss, format};

    auto content = std::string{};

    for (const auto& col_header : csv.get_col_names())
    {
        content += "(";
        content += col_header;
        content += ")";
    }

    content += " H ";

    for (const auto& row : csv)
    {
        if (vsl::is_csv_row_empty(row))
        {
            content += "empty | ";
            continue;
        }

        for (auto& cell : row)
        {
            content += "[";
            content += cell.get_sv();
            content += "]";
        }
        content += " | ";
    }

    return content;
}

static auto test_all(const vsl::CsvFormat& format, vsl::cstring expected_res) -> void
{
    ASSERT_EQ(process_text(format, TEXT_LF), expected_res);
    ASSERT_EQ(process_text(format, TEXT_CRLF), expected_res);
    ASSERT_EQ(process_text(format, TEXT_LF_BOM), expected_res);
}

TEST(CsvTest, WithHeader)
{
    auto format = get_default_format();
    format.header_row(0);

    const auto expected_res = "(col1)(col2) H [val1][001] | [val2][002] | [val5][005] | ";

    ASSERT_NO_FATAL_FAILURE(test_all(format, expected_res));
}

TEST(CsvTest, NoHeader)
{
    auto format = get_default_format();
    format.no_header();
    // format.variable_columns(vsl::CsvVariableColumnPolicy::KEEP);  // default

    const auto expected_res = " H [col1][col2] | [val1][001] | empty | [val2][002] | empty | [val3] | "
                              "[val4][004][xxx] | [val5][005] | ";

    ASSERT_NO_FATAL_FAILURE(test_all(format, expected_res));
}

TEST(CsvTest, NoHeaderKeepNonEmpty)
{
    auto format = get_default_format();
    format.no_header();
    format.variable_columns(vsl::CsvVariableColumnPolicy::KEEP_NON_EMPTY);

    const auto expected_res =
        " H [col1][col2] | [val1][001] | empty | [val2][002] | [val3] | [val4][004][xxx] | [val5][005] | ";

    ASSERT_NO_FATAL_FAILURE(test_all(format, expected_res));
}

TEST(CsvTest, NoHeaderIgnoreRow)
{
    auto format = get_default_format();
    format.no_header();
    format.variable_columns(vsl::CsvVariableColumnPolicy::IGNORE_ROW);

    const auto expected_res = " H [col1][col2] | [val1][001] | [val2][002] | [val5][005] | ";

    ASSERT_NO_FATAL_FAILURE(test_all(format, expected_res));
}

TEST(CsvTest, NoHeaderThrow)
{
    constexpr auto TEXT_TOO_SHORT = "val1;001\nval2\nval3;003";
    constexpr auto TEXT_TOO_LONG = "val1;001\nval2;003;xxx\nval3;003";

    auto format = get_default_format();
    format.no_header();
    format.variable_columns(vsl::CsvVariableColumnPolicy::THROW);

    ASSERT_THAT([&] { process_text(format, TEXT_TOO_SHORT); },
                ThrowsMessage<std::runtime_error>(HasSubstr("Line too short")));

    ASSERT_THAT([&] { process_text(format, TEXT_TOO_LONG); },
                ThrowsMessage<std::runtime_error>(HasSubstr("Line too long")));
}

}  // namespace test
