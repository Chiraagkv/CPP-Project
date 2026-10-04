#pragma once

#include <functional>
#include <string>
#include <vector>

using namespace std;

class DataSet;

class Filter {
private:
    string column;
    function<bool(double)> condition;

public:
    Filter(string columnName, function<bool(double)> cond);

    const string& getColumn() const { return column; }
    vector<size_t> matchingRows(const DataSet& ds) const;
};
