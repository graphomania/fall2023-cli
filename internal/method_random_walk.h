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

public:
    explicit RandomWalk(const Log&logger, const double tolerance = 1e-5,
                        const size_t min = 100, const size_t max = 10000)
        : Method(logger),
          min_(min),
          max_(max),
          tolerance_(tolerance) {
    }

    [[nodiscard]] std::string name() const override { return "Random Walk method"; }

    [[nodiscard]] Function::Value minimal(Function* func, const Area&where) const override;
};

#endif //RANDOM_WALK_H
