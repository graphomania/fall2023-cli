#ifndef FUNCTION_EXAMPLES_H
#define FUNCTION_EXAMPLES_H


#include <cassert>

#include "common.h"

#include <limits>


template <typename Ret, typename T, typename F=std::function<Ret(T)>>
std::vector<Ret> map(const std::vector<T>& from, const F& func) {
    auto ret = std::vector<Ret>(from.size());
    for (size_t i = 0; i < from.size(); i++) {
        ret[i] = func(from[i]);
    }
    return ret;
}

class FunctionI {
public:
    using Value = std::pair<Point, double>;

    virtual ~FunctionI() = default;

    virtual double operator()(const Point& point) const = 0;

    [[nodiscard]] virtual std::vector<FunctionI::Value> minimal() const = 0;

    [[nodiscard]] virtual std::vector<FunctionI::Value> maximum() const = 0;

    [[nodiscard]] virtual bool is_dimensions_supported(size_t n) const = 0;

    [[nodiscard]] virtual std::string name() const = 0;

    [[nodiscard]] FunctionI::Value closest_minimal(const Point& point) const {
        const auto point_extended = point.appended(this->operator()(point));
        auto min_dist = std::numeric_limits<double>::max();
        Point ret;
        for (auto [min_point, min_value] : minimal()) {
            if (const auto dist = min_point.appended(min_value).dist(point_extended); min_dist > dist) {
                min_dist = dist;
                ret = min_point;
            }
        }
        return {ret, operator()(ret)};
    }

    [[nodiscard]] double MSE(const std::vector<Point>& lhs, const std::vector<Point>& rhs) const {
        assert(lhs.size() == rhs.size());

        double ret = 0;
        for (size_t i = 0; i < lhs.size(); i++) {
            auto left = lhs[i].appended(operator()(lhs[i]));
            auto right = rhs[i].appended(operator()(rhs[i]));
            ret += sqr(left.dist(right));
        }
        return std::sqrt(ret / lhs.size());
    }
};

class Function : public FunctionI {
protected:
    size_t dimensions_;
    std::vector<Point> known_min_;
    std::vector<Point> known_max_;

public:
    Function(const size_t n, std::vector<Point> min, std::vector<Point> max)
        : dimensions_(n), known_min_(std::move(min)), known_max_(std::move(max)) {}

    [[nodiscard]] std::vector<FunctionI::Value> minimal() const override {
        return map<Function::Value>(known_min_, [func=this](const Point& x)-> Function::Value {
            return {x, func->operator()(x)};
        });
    }

    [[nodiscard]] std::vector<FunctionI::Value> maximum() const override {
        return map<Function::Value>(known_max_, [func=this](const Point& x)-> Function::Value {
            return {x, func->operator()(x)};
        });
    }

    [[nodiscard]] bool is_dimensions_supported(size_t n) const override {
        if (dimensions_ == 0) {
            return true;
        }
        return n == dimensions_;
    }
};


// RastriginFunction: https://en.wikipedia.org/wiki/Test_functions_for_optimization
class RastriginFunction final : public Function {
    size_t size_;

public:
    explicit RastriginFunction(size_t n) : Function(n, {{std::vector(n, 0.0)}}, {}), size_(n) {
        if (n > 16 || n < 1) {
            throw std::invalid_argument(
                fmt::format("RastriginFunction: dimension={} is too big, should be between 1 and 16", n)
            );
        }

        known_max_ = {{}};
        for (size_t i = 0; i < n; i++) {
            auto next_known_max = std::vector<Point>{};
            for (const auto& point : known_max_) {
                next_known_max.push_back(point.appended(4.5229936666666));
            }
            known_max_ = std::move(next_known_max);
        }
    }

    double operator()(const Point& point) const override {
        constexpr double A = 10;
        double ret = A * point.size();
        for (auto x : point) {
            ret += sqr(x) - A * std::cos(2 * 3.14 /*good enough*/ * x);
        }
        return ret;
    }

    [[nodiscard]] std::string name() const override {
        return std::string{"Rastrigin function [f(x) = 10n + \\sum_{i=1}^{"} +
            fmt::format("{}", size_) + "} (x_i^2 - 10 * cos(2 \\pi x_i))]";
    }
};

// HimmelblauFunction: https://en.wikipedia.org/wiki/Himmelblau%27s_function
class HimmelblauFunction final : public Function {
public:
    explicit HimmelblauFunction(size_t n = 2)
        : Function(n,
                   {
                       {{3.0, 2.0}},
                       {{-2.805118, 3.131312}},
                       {{-3.779310, -3.283186}},
                       {{3.584428, -1.848126}}
                   },
                   {{{-0.270845, -0.923039}}}
        ) {
        if (n != 2) {
            throw std::invalid_argument(
                fmt::format("HimmelblauFunction: dimension={} is invalid, should be exactly 2", n)
            );
        }
    }

    double operator()(const Point& point) const override {
        if (point.size() != 2) {
            throw std::invalid_argument(
                fmt::format("HimmelblauFunction::call: point.dimension={} is invalid, should be exactly 2",
                            point.size())
            );
        }
        return sqr(sqr(point[0]) + point[1] - 11) + sqr(point[0] + sqr(point[1]) - 7);
    }

    [[nodiscard]] std::string name() const override {
        return "Himmelblau function [f(x, y) = (x^2 + y - 11)^2 + (x + y^2 - 7)^2]";
    }
};


// Styblinski–Tang function: https://en.wikipedia.org/wiki/File:Goldstein_Price_function.pdf
class StyblinskiTangFunction final : public Function {
public:
    explicit StyblinskiTangFunction(size_t n = 2)
        : Function(n,
                   {
                       {{-2.903534, -2.903534}}
                   },
                   {}
        ) {
        if (n != 2) {
            throw std::invalid_argument(
                fmt::format("Styblinski–Tang: dimension={} is invalid, should be exactly 2", n)
            );
        }
    }

    double operator()(const Point& point) const override {
        if (point.size() != 2) {
            throw std::invalid_argument(
                fmt::format("Styblinski–Tang::call: point.dimension={} is invalid, should be exactly 2",
                            point.size())
            );
        }
        const auto x = point[0];
        const auto y = point[1];
        return ((sqr(sqr(x)) - 16 * sqr(x) + 5 * x) + (sqr(sqr(y)) - 16 * sqr(y) + 5 * y)) / 2 + 80;
    }

    [[nodiscard]] std::string name() const override {
        return
            R"(Styblinski–Tang function [f(x, y) = \sum_{i=0}^n{x_i^4 - 16x_i^2 + 5x_i} / 2])";
    }
};


#endif //FUNCTION_EXAMPLES_H
