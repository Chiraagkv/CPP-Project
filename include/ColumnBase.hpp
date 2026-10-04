#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

class ColumnBase {
public:
    virtual ~ColumnBase() = default;

    virtual string getName() const = 0;
    virtual string typeName() const = 0;
    virtual size_t size() const = 0;
    virtual void print() const = 0;

    virtual bool isNumeric() const = 0;
    virtual double getAsDouble(size_t i) const = 0;
    virtual string getAsString(size_t i) const = 0;

    virtual unique_ptr<ColumnBase> select(const vector<size_t>& rows) const = 0;
};
