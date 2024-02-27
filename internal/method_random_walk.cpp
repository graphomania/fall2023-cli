#include "method_random_walk.h"

#include "random.h"

#include <optional>


Function::Value RandomWalk::minimal(Function* func, const Area& where) const {
    steps_ = 0;
    std::optional<Function::Value> min;
    for (size_t iter = 1; iter < max_; iter++) {
        steps_ += 1;
        Point point;
        if (min.has_value() && random::gen(0, 1) <= p_) {
            point = min.value().first.uniformly_deviate(-delta_, delta_);
        } else {
            point = where.random_point();
        }
        const auto value = (*func)(point);
        if (!min.has_value()) {
            min = {std::move(point), value};
            log_.counted().info(fmt::format("{}, func value \t{},\t on iteration #{}",
                                            min->first, min->second, iter));
            continue;
        }

        if (abs(min.value().second - value) < tolerance_ && min_ <= iter) {
            log_.counted().info(fmt::format("EXITING: (|x_0 - x_n| < tolerance),\t on iteration #{}",
                                            min->first, min->second, iter));
            return min.value();
        }

        if (value < min.value().second) {
            min = {point, value};
            log_.counted().info(fmt::format("{}, func value \t{},\t on iteration #{}", min->first,
                                            min->second, iter));
        }
    }

    log_.counted().info(fmt::format("EXITING: iterations maximum has been reached"));
    return min.value();
}
