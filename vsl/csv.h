#ifndef VSL_CSV_H
#define VSL_CSV_H

#include <vincentlaucsb-csv-parser/csv.hpp>

// Reference:
// https://vincentlaucsb.github.io/csv-parser/

namespace vsl
{

using CsvDataType = csv::DataType;
using CsvColumnNamePolicy = csv::ColumnNamePolicy;
using CsvVariableColumnPolicy = csv::VariableColumnPolicy;
using CsvConversionError = csv::CSVConversionError;

using CsvFormat = csv::CSVFormat;
using CsvReader = csv::CSVReader;
using CsvRow = csv::CSVRow;
using CsvField = csv::CSVField;

// TODO: add from vincentlaucsb-csv-parser
// * DelimWriter
// * CSVWriter
// * CSVFileInfo
// * DataFrame
// * parse()
// * get_file_info()
// * get_col_names()
// * ...

inline auto is_csv_row_empty(const CsvRow& row) -> bool
{
    const auto is_single_empty_cell = (row.size() == 1 && row[0].is_null());
    return (row.empty() || is_single_empty_cell);
}

}  // namespace vsl

#endif  // VSL_CSV_H
