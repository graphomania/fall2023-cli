#include "method_random_walk.h"

#include <optional>

Point min_with_random_walk(const Area&area, const Function&func, const double tolerance,
                           const size_t min_iterations, const size_t max_iterations) {
    std::optional<std::pair<Point, double>> min;
    for (size_t iter = 0; min_iterations <= iter || iter < max_iterations; iter++) {
        auto point = area.random_point();
        const auto value = func(point);
        if (!min.has_value()) {
            min = {std::move(point), value};
            continue;
        }

        if (abs(min.value().second - value) < tolerance) {
            return min.value().first;
        }

        if (value < min.value().second) {
            min = {point, value};
        }
    }
    return min.value().first;
}

Function::Value RandomWalk::minimal(Function* func, const Area&where) const {
    steps_ = 0;
    std::optional<Function::Value> min;
    for (size_t iter = 1; iter < max_; iter++) {
        steps_ += 1;
        auto point = where.random_point();
        const auto value = (*func)(point);
        if (!min.has_value()) {
            min = {std::move(point), value};
            log_.counted().info(fmt::format("{}, func value \t{},\t on iteration #{}", min->first, min->second, iter));
            continue;
        }

        if (abs(min.value().second - value) < tolerance_ && min_ <= iter) {
            return min.value();
        }

        if (value < min.value().second) {
            min = {point, value};
            log_.counted().info(fmt::format("{}, func value \t{},\t on iteration #{}", min->first, min->second, iter));
        }
    }
    return min.value();
}
