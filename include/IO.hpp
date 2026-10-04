#pragma once

#include "DataSet.hpp"

#include <string>

using namespace std;

class FileFormat {
public:
    virtual ~FileFormat() = default;
    virtual string extension() const = 0;

protected:
    void checkExtension(const string& path) const;
};

class IImporter : public virtual FileFormat {
public:
    virtual DataSet importFrom(const string& path) const = 0;
};

class IExporter : public virtual FileFormat {
public:
    virtual void exportTo(const DataSet& ds, const string& path) const = 0;
};

class CSVHandler : public IImporter, public IExporter {
private:
    char delimiter;

public:
    explicit CSVHandler(char delim = ',') : delimiter(delim) {}

    string extension() const override { return ".csv"; }
    DataSet importFrom(const string& path) const override;
    void exportTo(const DataSet& ds, const string& path) const override;
};

class JSONExporter : public IExporter {
public:
    string extension() const override { return ".json"; }
    void exportTo(const DataSet& ds, const string& path) const override;
};
