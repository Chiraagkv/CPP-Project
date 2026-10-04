#pragma once

#include "Column.hpp"
#include "Filter.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace std;

class DataSet {
private:
    vector<unique_ptr<ColumnBase>> columns;

public:
    DataSet() = default;
    DataSet(DataSet&&) = default;
    DataSet& operator=(DataSet&&) = default;

    void addColumn(unique_ptr<ColumnBase> col);
    ColumnBase* getColumn(const string& name);
    const ColumnBase* getColumn(const string& name) const;

    template <typename T>
    Column<T>& getColumnAs(const string& name) {
        auto* typed = dynamic_cast<Column<T>*>(getColumn(name));
        if (!typed) throw TypeMismatchException(name, TypeName<T>::get());
        return *typed;
    }

    size_t rowCount() const;
    size_t columnCount() const { return columns.size(); }
    vector<string> columnNames() const;
    const ColumnBase& columnAt(size_t i) const;

    void loadCSV(const string& filename);

    DataSet filter(const Filter& f) const;
    DataSet filterBy(const string& colName, auto condition) const {
        return filter(Filter(colName, condition));
    }

    void print() const;
};
