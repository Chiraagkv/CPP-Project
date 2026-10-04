#pragma once

#include "Condition.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace std;

class DataSet;

class Filter {
private:
    shared_ptr<const Condition> condition;

public:
    template <ConditionType C>
    Filter(C cond) : condition(make_shared<C>(move(cond))) {}

    vector<size_t> matchingRows(const DataSet& ds, const string& columnName) const;
};
