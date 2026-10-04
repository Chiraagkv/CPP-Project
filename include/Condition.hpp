#pragma once

#include <concepts>
#include <functional>
#include <memory>

using namespace std;

class Condition {
public:
    virtual ~Condition() = default;
    virtual bool test(double value) const = 0;
};

template <typename C>
concept ConditionType = derived_from<C, Condition>;

class GreaterThan : public Condition {
private:
    double threshold;

public:
    explicit GreaterThan(double t) : threshold(t) {}
    bool test(double value) const override;
};

class LessThan : public Condition {
private:
    double threshold;

public:
    explicit LessThan(double t) : threshold(t) {}
    bool test(double value) const override;
};

class Equals : public Condition {
private:
    double target;

public:
    explicit Equals(double t) : target(t) {}
    bool test(double value) const override;
};

// Inclusive on both ends
class Between : public Condition {
private:
    double low, high;

public:
    Between(double lo, double hi);
    bool test(double value) const override;
};

class AndCondition : public Condition {
private:
    shared_ptr<const Condition> left, right;

public:
    template <ConditionType L, ConditionType R>
    AndCondition(L l, R r)
        : left(make_shared<L>(move(l))), right(make_shared<R>(move(r))) {}
    bool test(double value) const override;
};

class OrCondition : public Condition {
private:
    shared_ptr<const Condition> left, right;

public:
    template <ConditionType L, ConditionType R>
    OrCondition(L l, R r)
        : left(make_shared<L>(move(l))), right(make_shared<R>(move(r))) {}
    bool test(double value) const override;
};

class NotCondition : public Condition {
private:
    shared_ptr<const Condition> inner;

public:
    template <ConditionType C>
    explicit NotCondition(C c) : inner(make_shared<C>(move(c))) {}
    bool test(double value) const override;
};

// Escape hatch for conditions not covered by the classes above
class Predicate : public Condition {
private:
    function<bool(double)> fn;

public:
    explicit Predicate(function<bool(double)> f);
    bool test(double value) const override;
};
