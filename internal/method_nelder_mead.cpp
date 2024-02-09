#include "method_nelder_mead.h"

#include <ranges>
#include <algorithm>

using namespace std;


Point centroid(const std::vector<Point>&polygon) {
    auto ret = polygon[0];
    for (size_t i = 1; i < polygon.size(); i++) {
        ret = ret + polygon[i];
    }
    return ret / polygon.size();
}

template<typename T>
std::vector<T> sub_vector(const std::vector<T>&vec, size_t begin, size_t end) {
    return {vec.begin() + begin, vec.end() - (vec.size() - end)};
}

Function::Value NelderMead::minimal_internal(Function* func, vector<Point>&x) const {
    auto return_or_go_deeper = [&, prev_x = x]() -> Function::Value {
        log_.debug(fmt::format("({}, {}) {} {}", x.size(), prev_x.size(), x, prev_x));
        const auto mse = func->MSE(x, prev_x);
        if (mse < tolerance_) {
            return {x[0], (*func)(x[0])};
        }
        log_.counted().info(fmt::format("{}\t -> {} (MSE: {})", x, prev_x, mse));
        return minimal_internal(func, x);
    };

    // 1. Order
    ranges::sort(x, [&](const auto&lhs, const auto&rhs) -> bool {
        return (*func)(lhs) < (*func)(rhs);
    });

    // 2. Calculate x_o, the centroid of all points except x_n+1
    log_.debug("Nelder calc x_0");
    const auto x_o = centroid(sub_vector(x, 0, x.size() - 1));

    // 3. Reflection
    log_.debug("Nelder reflection");
    const auto x_r = x_o + (x_o - x.back()) * alpha_;
    if ((*func)(x[0]) <= (*func)(x_r) && (*func)(x_r) < (*func)(x[x.size() - 1])) {
        x.back() = x_r;
        return return_or_go_deeper();
    }

    // 4. Expansion
    log_.debug("Nelder expansion");
    if ((*func)(x_r) < (*func)(x[0])) {
        if (const auto x_e = x_o + (x_r - x_o) * gamma_; (*func)(x_e) < (*func)(x_r)) {
            x.back() = x_e;
            return return_or_go_deeper();;
        }

        x.back() = x_r;
        return return_or_go_deeper();
    }

    // 5. Contraction
    log_.debug("Nelder contraction");
    if ((*func)(x_r) >= (*func)(x[x.size() - 1])) {
        if ((*func)(x_r) < (*func)(x.back())) {
            if (const auto x_c = x_o + (x_r - x_o) * rho_; (*func)(x_c) < (*func)(x_r)) {
                x.back() = x_c;
                log_.debug("Nelder contraction (*func)(x_r) < (*func)(x.back()) & (*func)(x_c) < (*func)(x_r)");
                return return_or_go_deeper();
            }
        }
        else {
            if (const auto x_c = x_o + (x.back() - x_o) * rho_; (*func)(x_c) < (*func)(x_r)) {
                x.back() = x_c;
                log_.debug("Nelder contraction (*func)(x_r) >= (*func)(x.back()) & (*func)(x_c) < (*func)(x_r)");
                return return_or_go_deeper();
            }
        }
    }

    // 6. Shrink
    log_.debug("Nelder shrinking");
    for (size_t i = 1; i < x.size(); i++) {
        x[i] = x[0] + (x[i] - x[0]) * sigma_;
    }
    return return_or_go_deeper();
}

Function::Value NelderMead::minimal_internal_(Function* function, std::vector<Point>&x) const {
    auto func = [function](const auto&p) { return (*function)(p); };
    auto return_or_go_deeper = [&, prev_x = x]() -> Function::Value {
        steps_ += 1;
        auto mse = function->MSE(x, prev_x);
        if (mse < tolerance_) {
            return {x[0], func(x[0])};
        }
        log_.counted().info(fmt::format("{}\t -> {} (MSE: {})", x, prev_x, mse));
        return minimal_internal_(function, x);
    };

    // 1. Order
    sort(x.begin(), x.end(), [&](const auto&lhs, const auto&rhs) -> bool {
        return func(lhs) < func(rhs);
    });

    // 2. Calculate x_o, the centroid of all points except x_n+1
    auto x_o = centroid(sub_vector(x, 0, x.size() - 1));

    // 3. Reflection
    auto x_r = x_o + (x_o - x.back()) * alpha_;
    if (func(x[0]) <= func(x_r) && func(x_r) < func(x[x.size() - 2])) {
        x.back() = x_r;
        return return_or_go_deeper();
    }

    // 4. Expansion
    if (func(x_r) < func(x[0])) {
        auto x_e = x_o + (x_r - x_o) * gamma_;
        if (func(x_e) < func(x_r)) {
            x.back() = x_e;
            return return_or_go_deeper();;
        }
        else {
            x.back() = x_r;
            return return_or_go_deeper();
        }
    }

    // 5. Contraction
    if (func(x_r) >= func(x[x.size() - 2])) {
        if (func(x_r) < func(x.back())) {
            if (auto x_c = x_o + (x_r - x_o) * rho_; func(x_c) < func(x_r)) {
                x.back() = x_c;
                return return_or_go_deeper();
            }
        }
        else {
            if (auto x_c = x_o + (x.back() - x_o) * rho_; func(x_c) < func(x_r)) {
                x.back() = x_c;
                return return_or_go_deeper();
            }
        }
    }

    // 6. Shrink
    for (size_t i = 1; i < x.size(); i++) {
        x[i] = x[0] + (x[i] - x[0]) * sigma_;
    }
    return return_or_go_deeper();
}
