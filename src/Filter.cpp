#include "Filter.hpp"
#include "DataSet.hpp"

using namespace std;

vector<size_t> Filter::matchingRows(const DataSet& ds, const string& columnName) const {
    const ColumnBase* col = ds.getColumn(columnName);
    if (!col->isNumeric()) throw TypeMismatchException(columnName, "numeric");

    vector<size_t> rows;
    for (size_t i = 0; i < col->size(); ++i)
        if (condition->test(col->getAsDouble(i))) rows.push_back(i);
    return rows;
}
