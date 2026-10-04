#pragma once

#include "DataSet.hpp"
#include "IO.hpp"

#include <string>

using namespace std;

class IVisualizer {
public:
    virtual ~IVisualizer() = default;
    virtual void plot(const DataSet& ds, const string& xCol, const string& yCol,
                      const string& path) const = 0;
};

class ScatterPlot : public IVisualizer, public FileFormat {
private:
    int width;
    int height;
    string color;

public:
    ScatterPlot(int w = 640, int h = 480, string pointColor = "#4C78A8");

    string extension() const override { return ".svg"; }
    void plot(const DataSet& ds, const string& xCol, const string& yCol,
              const string& path) const override;
};
