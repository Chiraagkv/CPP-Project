#include "Filter.hpp"
#include "DataSet.hpp"

using namespace std;

Filter::Filter(string columnName, function<bool(double)> cond)
    : column(move(columnName)), condition(move(cond)) {
    if (!condition) throw DataException("Filter condition is empty");
}

vector<size_t> Filter::matchingRows(const DataSet& ds) const {
    const ColumnBase* col = ds.getColumn(column);
    if (!col->isNumeric()) throw TypeMismatchException(column, "numeric");

    vector<size_t> rows;
    for (size_t i = 0; i < col->size(); ++i)
        if (condition(col->getAsDouble(i))) rows.push_back(i);
    return rows;
}
