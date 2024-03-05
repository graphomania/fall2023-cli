#include "method_random_walk.h"

#include "random.h"

#include <optional>


Function::Value RandomWalk::minimal_internal(Function* func, const Area& where, std::vector<Point>& path) const {
    steps_ = 0;
    std::optional<Function::Value> min;
    if (start_.has_value()) {
        min = {start_.value(), (*func)(start_.value())};
        path.push_back(start_.value());
    }
    for (size_t iter = 1; iter < max_; iter++) {
        steps_ += 1;
        Point point;
        if (min.has_value() && random::with_chance(p_)) {
            point = min.value().first.uniformly_deviate(-delta_, delta_);
        } else {
            point = where.random_point();
        }
        const auto value = (*func)(point);
        if (!min.has_value()) {
            path.push_back(point);
            min = {std::move(point), value};
            log_.counted().info(fmt::format("{}, func value \t{},\t on iteration #{}",
                                            min->first, min->second, iter));
            continue;
        }

        if (abs(min.value().second - value) < tolerance_ && min_ <= iter) {
            path.push_back(point);
            log_.counted().info(fmt::format("EXITING: (|x_0 - x_n| < tolerance),\t on iteration #{}",
                                            min->first, min->second, iter));
            return min.value();
        }

        if (value < min.value().second) {
            path.push_back(point);
            min = {point, value};
            log_.counted().info(fmt::format("{}, func value \t{},\t on iteration #{}", min->first,
                                            min->second, iter));
        }
    }

    log_.counted().info(fmt::format("EXITING: iterations maximum has been reached"));
    return min.value();
}

std::pair<std::vector<Point>, Function::Value> RandomWalk::minimal_with_path(Function* func, const Area& where) const {
    std::vector<Point> path;
    auto ret = minimal_internal(func, where, path);
    return {path, ret};
}

Function::Value RandomWalk::minimal(Function* func, const Area& where) const {
    return minimal_with_path(func, where).second;
}
