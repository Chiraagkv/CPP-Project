#include "Analyzer.hpp"
#include "Exceptions.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>

using namespace std;

vector<double> IAnalyzer::extractNumbers(const ColumnBase& col) {
    if (!col.isNumeric()) throw TypeMismatchException(col.getName(), "numeric");
    if (col.size() == 0) throw EmptyDataException("column '" + col.getName() + "' is empty");

    vector<double> nums;
    nums.reserve(col.size());
    for (size_t i = 0; i < col.size(); ++i) nums.push_back(col.getAsDouble(i));
    return nums;
}

double MeanAnalyzer::analyze(const ColumnBase& col) const {
    auto nums = extractNumbers(col);
    return accumulate(nums.begin(), nums.end(), 0.0) / nums.size();
}

double MedianAnalyzer::analyze(const ColumnBase& col) const {
    auto nums = extractNumbers(col);
    sort(nums.begin(), nums.end());
    size_t n = nums.size();
    return n % 2 ? nums[n / 2] : (nums[n / 2 - 1] + nums[n / 2]) / 2.0;
}

void StatisticsEngine::addAnalyzer(unique_ptr<IAnalyzer> analyzer) {
    if (!analyzer) throw DataException("Cannot add a null analyzer");
    analyzers.push_back(move(analyzer));
}

void StatisticsEngine::report(const ColumnBase& col) const {
    cout << "Statistics for '" << col.getName() << "':\n";
    for (const auto& a : analyzers)
        cout << "  " << a->name() << ": " << a->analyze(col) << "\n";
}
