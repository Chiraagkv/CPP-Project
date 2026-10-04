#include "Analyzer.hpp"
#include "DataSet.hpp"
#include "IO.hpp"
#include "Visualizer.hpp"

#include <iostream>

using namespace std;

void section(const string& title) {
    cout << "\n=== " << title << " ===\n";
}

int main() {
    try {
        section("Build a DataSet by hand");
        DataSet manual;
        auto scores = make_unique<Column<double>>("score");
        auto ids = make_unique<Column<int>>("id");
        for (int i = 1; i <= 5; ++i) {
            ids->addValue(i);
            scores->addValue(i * 1.5);
        }
        manual.addColumn(move(ids));
        manual.addColumn(move(scores));
        manual.print();

        section("Load CSV");
        DataSet ds;
        ds.loadCSV("data/employees.csv");
        ds.print();

        section("Polymorphic column access");
        for (size_t i = 0; i < ds.columnCount(); ++i) {
            const ColumnBase& col = ds.columnAt(i);
            cout << col.getName() << " -> " << col.typeName() << ", " << col.size() << " values\n";
        }

        section("Typed access via template");
        auto& ages = ds.getColumnAs<int>("age");
        cout << "First age: " << ages.at(0) << "\n";

        section("Strategy pattern analytics");
        StatisticsEngine engine;
        engine.addAnalyzer(make_unique<MeanAnalyzer>());
        engine.addAnalyzer(make_unique<MedianAnalyzer>());
        engine.report(*ds.getColumn("salary"));
        engine.report(*ds.getColumn("age"));

        section("Filter: age > 30");
        DataSet older = ds.filterBy("age", [](double a) { return a > 30; });
        older.print();

        section("Scatter plot");
        ScatterPlot plot;
        const IVisualizer& viz = plot;
        viz.plot(ds, "experience", "salary", "scatter.svg");
        cout << "Wrote scatter.svg\n";

        section("Export");
        CSVHandler csv;
        JSONExporter json;
        vector<const IExporter*> exporters = {&csv, &json};
        for (const IExporter* e : exporters) {
            string path = "output" + e->extension();
            e->exportTo(older, path);
            cout << "Wrote " << path << "\n";
        }

        section("Exception handling");
        try {
            ds.getColumn("bonus");
        } catch (const ColumnNotFoundException& e) {
            cout << "Caught ColumnNotFoundException: " << e.what() << "\n";
        }
        try {
            MeanAnalyzer().analyze(*ds.getColumn("name"));
        } catch (const TypeMismatchException& e) {
            cout << "Caught TypeMismatchException: " << e.what() << "\n";
        }
        try {
            ds.getColumnAs<double>("age");
        } catch (const TypeMismatchException& e) {
            cout << "Caught TypeMismatchException: " << e.what() << "\n";
        }
        try {
            DataSet missing;
            missing.loadCSV("data/missing.csv");
        } catch (const FileException& e) {
            cout << "Caught FileException: " << e.what() << "\n";
        }
        try {
            plot.plot(ds, "experience", "salary", "scatter.png");
        } catch (const FileException& e) {
            cout << "Caught FileException: " << e.what() << "\n";
        }
        try {
            manual.addColumn(make_unique<Column<int>>("id"));
        } catch (const DataException& e) {
            cout << "Caught DataException: " << e.what() << "\n";
        }
    } catch (const DataException& e) {
        cerr << "Data error: " << e.what() << "\n";
        return 1;
    } catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
