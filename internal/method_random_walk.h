#ifndef RANDOM_WALK_H
#define RANDOM_WALK_H

#include <optional>
#include <utility>

#include "common.h"
#include "log.h"
#include "method.h"


class RandomWalk final : public Method {
    size_t min_, max_;
    double tolerance_;
    const double delta_;
    const double p_;

public:
    explicit RandomWalk(const Log& logger, const double delta = 0.1, const double p = 0.2,
                        const double tolerance = 1e-5, const size_t min = 100, const size_t max = 10000)
        : Method(logger.with("RandomWalk")),
          min_(min), max_(max), tolerance_(tolerance),
          delta_(delta), p_(p) {}

    [[nodiscard]]
    std::string name() const override { return "Random Walk method"; }

    Function::Value minimal_internal(Function* func, const Area& where, std::vector<Point>& path) const;
    [[nodiscard]]
    Function::Value minimal(Function* func, const Area& where) const override;

    [[nodiscard]]
    std::pair<std::vector<Point>, Function::Value>
    minimal_with_path(Function* func, const Area& where) const override;
};

#endif //RANDOM_WALK_H
