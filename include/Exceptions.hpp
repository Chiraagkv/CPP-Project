#pragma once

#include <stdexcept>
#include <string>

using namespace std;

class DataException : public runtime_error {
public:
    using runtime_error::runtime_error;
};

class ColumnNotFoundException : public DataException {
public:
    explicit ColumnNotFoundException(const string& name)
        : DataException("Column not found: " + name) {}
};

class TypeMismatchException : public DataException {
public:
    TypeMismatchException(const string& column, const string& expected)
        : DataException("Column '" + column + "' is not of type " + expected) {}
};

class EmptyDataException : public DataException {
public:
    explicit EmptyDataException(const string& what)
        : DataException("No data: " + what) {}
};

class FileException : public DataException {
public:
    explicit FileException(const string& msg)
        : DataException("File error: " + msg) {}
};
