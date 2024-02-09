#ifndef FUNCTION_H
#define FUNCTION_H

#include "random.h"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <random>
#include <vector>
#include <cmath>
#include <functional>
#include <string_view>
#include <stdexcept>
#include <sstream>


auto sqr(const auto&x) {
    return x * x;
}

auto abs(auto val) {
    if (val < 0) {
        return -val;
    }
    return val;
}


class Point : public std::vector<double> {
    void assert_sizes_match(const Point&other, std::string_view from) const {
        if (this->size() != other.size())
            throw std::invalid_argument(fmt::format("Point::{}: sizes doesn't match, {} != {}\n({}, {})", from,
                                                    size(), other.size(), *this, other));
    }

public:
    static Point random(const size_t dimension, const Point&min, const Point&max) {
        auto ret = Point{};
        ret.resize(dimension);
        for (size_t i = 0; i < dimension; i++) {
            ret[i] = random::gen(min[i], max[i]);
        }
        return ret;
    }

    static Point rep(const size_t n, const double val) {
        auto ret = Point{std::vector(n, val)};
        for (size_t i = 0; i < n; i++) {
            ret[i] = val;
        }
        return ret;
    }

    [[nodiscard]] double dist(const Point&to) const {
        assert_sizes_match(to, "to");

        double ret = 0;
        for (size_t i = 0; i < size(); i++)
            ret += std::sqrt(sqr(at(i) - to.at(i)));
        return ret;
    }

    [[nodiscard]] double dist_with(const Point&to, const std::function<double(const Point&x)>&func) const {
        return this->appended(func(*this)).dist(to.appended(func(to)));
    }

    Point operator+(const Point&other) const {
        assert_sizes_match(other, "operator+");

        auto ret = Point{};
        ret.resize(size());
        for (size_t i = 0; i < size(); i++) {
            ret[i] = at(i) + other.at(i);
        }
        return ret;
    }

    Point operator-(const Point&other) const {
        assert_sizes_match(other, "operator-");
        return *this + -other;
    }

    Point operator*(const double x) const {
        auto ret = Point{};
        ret.resize(size());
        for (size_t i = 0; i < size(); i++) {
            ret[i] = at(i) * x;
        }
        return ret;
    }

    Point operator/(const double x) const { return *this * (1 / x); }

    Point operator-() const { return *this * (-1); }

    [[nodiscard]] Point appended(const double x) const {
        auto copy = *this;
        copy.push_back(x);
        return copy;
    }
};

class Area {
    Point min_, max_;

public:
    static Area cube(const size_t dimensions, const double min, const double max) {
        return Area(Point(std::vector(dimensions, min)), Point(std::vector(dimensions, max)));
    }

    Area(Point min, Point max) : min_(std::move(min)), max_(std::move(max)) {
        if (min_.size() != max_.size()) {
            throw std::invalid_argument("min_.size() != max_.size()");
        }
        if (min_.size() == 0) {
            throw std::invalid_argument("min_.size() == 0");
        }
    }

    [[nodiscard]] bool contains(const Point&point) const {
        if (point.size() != dimensions()) {
            throw std::invalid_argument("the point is from other dimestion");
        }

        for (size_t i = 0; i < point.size(); i++) {
            if (min_[i] > point[i] || max_[i] < point[i]) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] Point random_point() const {
        return Point::random(min_.size(), min_, max_);
    }

    [[nodiscard]] std::vector<Point> border_vertexes() const {
        auto points = std::vector<Point>{{}};
        for (size_t i = 0; i < dimensions(); i++) {
            auto next = std::vector<Point>{};
            for (auto&point: points) {
                next.push_back(point.appended(min_[i]));
                next.push_back(point.appended(max_[i]));
            }
            points = std::move(next);
        }
        return points;
    }

    [[nodiscard]] size_t dimensions() const {
        return min_.size();
    }

    [[nodiscard]] std::string to_string() const {
        std::ostringstream oss;
        for (size_t i = 0; i < dimensions(); i++) {
            if (i != 0) {
                oss << " x ";
            }
            oss << '[' << min_[i] << ", " << max_[i] << ']';
        }
        return oss.str();
    }
};


#endif //FUNCTION_H
