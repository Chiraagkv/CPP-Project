#pragma once

#include "ColumnBase.hpp"
#include "Exceptions.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <type_traits>

using namespace std;

template <typename T> struct TypeName {
    static string get() {
        return "unknown";
    } 
};
template <> struct TypeName<int>{
    static string get() {
        return "int";
    }
};
template <> struct TypeName<double>{
    static string get() {
        return "double";
    }
};
template <> struct TypeName<string>{
    static string get() {
        return "string";
    }
};

template <typename T>
class Column : public ColumnBase {
private:
    string name;
    vector<T> data;

public:
    explicit Column(string n) : name(move(n)) {}

    void addValue(T val) {
        data.push_back(move(val));
    }

    const T& at(size_t i) const {
        if (i >= data.size())
            throw out_of_range("Index " + to_string(i) + " out of range in column '" + name + "'");
        return data[i];
    }

    const vector<T>& values() const { return data; }

    string getName() const override { return name; }
    string typeName() const override { return TypeName<T>::get(); }
    size_t size() const override { return data.size(); }
    bool isNumeric() const override { return is_arithmetic_v<T>; }

    double getAsDouble(size_t i) const override {
        if constexpr (is_arithmetic_v<T>)
            return static_cast<double>(at(i));
        else
            throw TypeMismatchException(name, "numeric");
    }

    string getAsString(size_t i) const override {
        if constexpr (is_same_v<T, string>) {
            return at(i);
        } else {
            ostringstream os;
            os << setprecision(15) << at(i);
            return os.str();
        }
    }

    unique_ptr<ColumnBase> select(const vector<size_t>& rows) const override {
        auto out = make_unique<Column<T>>(name);
        for (size_t r : rows) out->addValue(at(r));
        return out;
    }

    void print() const override {
        cout << name << " (" << typeName() << ") [";
        for (size_t i = 0; i < data.size(); ++i)
            cout << (i ? ", " : "") << data[i];
        cout << "]\n";
    }
};
