#ifndef NEDLER_MEAD_NELDER_MEAD_H
#define NEDLER_MEAD_NELDER_MEAD_H


#include "common.h"
#include "method.h"

#include <optional>


class NelderMead final : public Method {
    std::optional<std::vector<Point>> start_;
    double tolerance_;
    double alpha_;
    double gamma_;
    double rho_;
    double sigma_;

    // https://en.wikipedia.org/wiki/Nelder–Mead_method
    // alpha > 0
    // gamma > 1
    // 0 < rho <= 0.5
    Function::Value minimal_internal(Function* func, std::vector<Point>& x) const;

    Function::Value minimal_internal_(Function* func, std::vector<Point>& x) const;

public:
    // If NedlerMeadMethod's (start == None) => (it's chosen randomly each run)
    explicit NelderMead(const Log& logger,
                        const double threashold = 0.01,
                        std::optional<std::vector<Point>> start = {},
                        const double alpha = 1,
                        const double gamma = 2,
                        const double rho = 0.5,
                        const double sigma = 0.5)
        : Method(logger.with("NelderMead")),
          start_(std::move(start)),
          tolerance_(threashold),
          alpha_(alpha),
          gamma_(gamma),
          rho_(rho),
          sigma_(sigma) {}

    NelderMead& with(std::vector<Point> start) {
        start_ = std::move(start);
        return *this;
    }

    [[nodiscard]] std::string name() const override { return "Nelder Mead method"; }

    [[nodiscard]] Function::Value minimal(Function* func, const Area& where) const override {
        auto x = std::vector<Point>{};
        if (start_.has_value()) {
            x = start_.value();
        } else {
            for (size_t i = 0; i <= where.dimensions(); i++) {
                x.push_back(where.random_point());
            }
        }

        steps_ = 0;
        return minimal_internal_(func, x);
    }
};


#endif //NEDLER_MEAD_NELDER_MEAD_H
