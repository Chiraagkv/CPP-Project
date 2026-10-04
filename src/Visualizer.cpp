#include "Visualizer.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

namespace {

const int MARGIN = 60;
const int TICKS = 5;

string fmt(double v) {
    ostringstream os;
    os << v;
    return os.str();
}

}

ScatterPlot::ScatterPlot(int w, int h, string pointColor)
    : width(w), height(h), color(move(pointColor)) {
    if (width <= 2 * MARGIN || height <= 2 * MARGIN)
        throw invalid_argument("Plot must be larger than " + to_string(2 * MARGIN) + "px each side");
}

void ScatterPlot::plot(const DataSet& ds, const string& xCol, const string& yCol,
                       const string& path) const {
    checkExtension(path);
    const ColumnBase* x = ds.getColumn(xCol);
    const ColumnBase* y = ds.getColumn(yCol);
    if (!x->isNumeric()) throw TypeMismatchException(xCol, "numeric");
    if (!y->isNumeric()) throw TypeMismatchException(yCol, "numeric");
    if (x->size() == 0) throw EmptyDataException("nothing to plot");

    vector<double> xs, ys;
    for (size_t i = 0; i < x->size(); ++i) {
        xs.push_back(x->getAsDouble(i));
        ys.push_back(y->getAsDouble(i));
    }

    auto [xMinIt, xMaxIt] = minmax_element(xs.begin(), xs.end());
    auto [yMinIt, yMaxIt] = minmax_element(ys.begin(), ys.end());
    double xMin = *xMinIt, xMax = *xMaxIt, yMin = *yMinIt, yMax = *yMaxIt;
    double xRange = xMax > xMin ? xMax - xMin : 1.0;
    double yRange = yMax > yMin ? yMax - yMin : 1.0;

    double plotW = width - 2 * MARGIN, plotH = height - 2 * MARGIN;
    auto px = [&](double v) { return MARGIN + (v - xMin) / xRange * plotW; };
    auto py = [&](double v) { return height - MARGIN - (v - yMin) / yRange * plotH; };

    ofstream out(path);
    if (!out) throw FileException("cannot write '" + path + "'");

    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width << "\" height=\"" << height
        << "\" font-family=\"sans-serif\" font-size=\"12\">\n"
        << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n"
        << "<text x=\"" << width / 2 << "\" y=\"25\" text-anchor=\"middle\" font-size=\"16\">"
        << yCol << " vs " << xCol << "</text>\n";

    for (int t = 0; t <= TICKS; ++t) {
        double xv = xMin + xRange * t / TICKS, yv = yMin + yRange * t / TICKS;
        out << "<line x1=\"" << px(xv) << "\" y1=\"" << MARGIN << "\" x2=\"" << px(xv) << "\" y2=\""
            << height - MARGIN << "\" stroke=\"#eee\"/>\n"
            << "<line x1=\"" << MARGIN << "\" y1=\"" << py(yv) << "\" x2=\"" << width - MARGIN << "\" y2=\""
            << py(yv) << "\" stroke=\"#eee\"/>\n"
            << "<text x=\"" << px(xv) << "\" y=\"" << height - MARGIN + 18
            << "\" text-anchor=\"middle\">" << fmt(xv) << "</text>\n"
            << "<text x=\"" << MARGIN - 6 << "\" y=\"" << py(yv) + 4 << "\" text-anchor=\"end\">"
            << fmt(yv) << "</text>\n";
    }

    out << "<line x1=\"" << MARGIN << "\" y1=\"" << height - MARGIN << "\" x2=\"" << width - MARGIN
        << "\" y2=\"" << height - MARGIN << "\" stroke=\"black\"/>\n"
        << "<line x1=\"" << MARGIN << "\" y1=\"" << MARGIN << "\" x2=\"" << MARGIN << "\" y2=\""
        << height - MARGIN << "\" stroke=\"black\"/>\n"
        << "<text x=\"" << width / 2 << "\" y=\"" << height - 15 << "\" text-anchor=\"middle\">" << xCol
        << "</text>\n"
        << "<text x=\"15\" y=\"" << height / 2 << "\" text-anchor=\"middle\" transform=\"rotate(-90 15 "
        << height / 2 << ")\">" << yCol << "</text>\n";

    for (size_t i = 0; i < xs.size(); ++i)
        out << "<circle cx=\"" << px(xs[i]) << "\" cy=\"" << py(ys[i]) << "\" r=\"5\" fill=\"" << color
            << "\" fill-opacity=\"0.8\"><title>(" << fmt(xs[i]) << ", " << fmt(ys[i])
            << ")</title></circle>\n";

    out << "</svg>\n";
}
