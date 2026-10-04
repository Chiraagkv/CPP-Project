#pragma once

#include "ColumnBase.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace std;

class IAnalyzer {
public:
    virtual ~IAnalyzer() = default;
    virtual double analyze(const ColumnBase& col) const = 0;
    virtual string name() const = 0;

protected:
    static vector<double> extractNumbers(const ColumnBase& col);
};

class MeanAnalyzer : public IAnalyzer {
public:
    double analyze(const ColumnBase& col) const override;
    string name() const override { return "Mean"; }
};

class MedianAnalyzer : public IAnalyzer {
public:
    double analyze(const ColumnBase& col) const override;
    string name() const override { return "Median"; }
};

class StatisticsEngine {
private:
    vector<unique_ptr<IAnalyzer>> analyzers;

public:
    void addAnalyzer(unique_ptr<IAnalyzer> analyzer);
    void report(const ColumnBase& col) const;
};
