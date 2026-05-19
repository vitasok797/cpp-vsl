#ifndef VSL_CSV_H
#define VSL_CSV_H

#include <vincentlaucsb-csv-parser/csv.hpp>

namespace vsl
{

// Reference:
// https://vincentlaucsb.github.io/csv-parser/

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

}  // namespace vsl

#endif  // VSL_CSV_H
