#include "Condition.hpp"
#include "Exceptions.hpp"

using namespace std;

bool GreaterThan::test(double value) const { return value > threshold; }

bool LessThan::test(double value) const { return value < threshold; }

bool Equals::test(double value) const { return value == target; }

Between::Between(double lo, double hi) : low(lo), high(hi) {
    if (low > high) throw DataException("Between: lower bound exceeds upper bound");
}

bool Between::test(double value) const { return value >= low && value <= high; }

bool AndCondition::test(double value) const { return left->test(value) && right->test(value); }

bool OrCondition::test(double value) const { return left->test(value) || right->test(value); }

bool NotCondition::test(double value) const { return !inner->test(value); }

Predicate::Predicate(function<bool(double)> f) : fn(move(f)) {
    if (!fn) throw DataException("Predicate function is empty");
}

bool Predicate::test(double value) const { return fn(value); }
