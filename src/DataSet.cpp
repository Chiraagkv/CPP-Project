#include "DataSet.hpp"
#include "IO.hpp"

using namespace std;

void DataSet::addColumn(unique_ptr<ColumnBase> col) {
    if (!col) throw DataException("Cannot add a null column");
    for (const auto& c : columns)
        if (c->getName() == col->getName())
            throw DataException("Duplicate column name: " + col->getName());
    if (!columns.empty() && col->size() != rowCount())
        throw DataException("Column '" + col->getName() + "' has " + to_string(col->size()) +
                            " rows, expected " + to_string(rowCount()));
    columns.push_back(move(col));
}

ColumnBase* DataSet::getColumn(const string& name) {
    for (auto& c : columns)
        if (c->getName() == name) return c.get();
    throw ColumnNotFoundException(name);
}

const ColumnBase* DataSet::getColumn(const string& name) const {
    for (const auto& c : columns)
        if (c->getName() == name) return c.get();
    throw ColumnNotFoundException(name);
}

size_t DataSet::rowCount() const {
    return columns.empty() ? 0 : columns.front()->size();
}

vector<string> DataSet::columnNames() const {
    vector<string> names;
    for (const auto& c : columns) names.push_back(c->getName());
    return names;
}

const ColumnBase& DataSet::columnAt(size_t i) const {
    if (i >= columns.size()) throw out_of_range("Column index out of range");
    return *columns[i];
}

void DataSet::loadCSV(const string& filename) {
    *this = CSVHandler().importFrom(filename);
}

DataSet DataSet::filter(const string& colName, const Filter& f) const {
    vector<size_t> rows = f.matchingRows(*this, colName);
    DataSet result;
    for (const auto& c : columns) result.addColumn(c->select(rows));
    return result;
}

void DataSet::print() const {
    cout << "DataSet: " << columns.size() << " columns x " << rowCount() << " rows\n";
    for (const auto& c : columns) {
        cout << "  ";
        c->print();
    }
}
