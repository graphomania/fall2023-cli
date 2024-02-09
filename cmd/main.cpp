#include "cli.h"
#include "../internal/common.h"
#include "../internal/method_random_walk.h"
#include "../internal/method_nelder_mead.h"
#include "../internal/function.h"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <memory>


Point joined(const Function::Value&val) {
    return val.first.appended(val.second);
}

std::string app(const Method* method, Function* func, const Area&area) {
    auto [min, min_val] = method->minimal(func, area);

    auto [closest, closest_val] = func->closest_minimal(min);
    return fmt::format("Method: {}, Function: {}\n"
                       "\tMin at x={}, f(x)={}.\n"
                       "\tThe closest known local minimum point: y={}, f(y)={}\n"
                       "\t||(x, f(x)) - (y, f(y))|| = {}))",
                       method->name(), func->name(),
                       min, min_val,
                       closest, closest_val,
                       min.dist_with(closest, [func](const auto&p) { return (*func)(p); })
    );
}

// void test_nelder() {
//     // auto ret = RandomWalk(Log(Log::Level::INFO)).minimal(new HimmelblauFunction, Area::cube(2, -6, 6));
//     auto ret = NelderMead(Log(Log::LEVEL::MUTED)).minimal(new HimmelblauFunction, Area::cube(2, -3, 3));
//
//     fmt::print("{}\n\r", ret);
//     fmt::print("and distance is {}\n\n", HimmelblauFunction());
//     // fmt::print("other minimals {}\n", HimmelblauFunction().minimal());
// }
//
// void test_random_walk() {
//     // auto ret = RandomWalk(Log(Log::Level::INFO)).minimal(new HimmelblauFunction, Area::cube(2, -6, 6));
//     auto ret = RandomWalk(Log(Log::LEVEL::INFO)).minimal(new HimmelblauFunction, Area::cube(2, -4, 4));
//
//     fmt::print("{}\n\r", ret);
//     fmt::print("and distance is {}\n", joined(HimmelblauFunction().closest_minimal(ret.first)).dist(joined(ret)));
// }
//
// void test_other() {
//     auto ret = NelderMead(Log(Log::LEVEL::INFO)).minimal(new RastriginFunction(3), Area::cube(3, -6, 6));
//
//     fmt::print("{}\n\r", ret);
//     fmt::print("and distance is {}\n\n", ret, RastriginFunction(3).closest_minimal(ret.first));
//     fmt::print("other minimals {}\n", RastriginFunction(3).minimal());
// }

int main(const int argc, char* argv[]) {
    return cli_app(argc, argv);

    // auto methods = std::vector{
    //     std::shared_ptr<Method>(std::make_shared<NelderMead>(Log(Log::LEVEL::MUTED))),
    //     std::shared_ptr<Method>(std::make_shared<RandomWalk>(Log(Log::LEVEL::MUTED))),
    // };
    //
    // auto functions = std::vector{
    //     std::shared_ptr<Function>(std::make_shared<HimmelblauFunction>()),
    //     std::shared_ptr<Function>(std::make_shared<RastriginFunction>(2)),
    //     // std::shared_ptr<Function>(std::make_shared<RastriginFunction>(3)),
    // };
    //
    //
    // for (auto method: methods) {
    //     for (auto function: functions) {
    //         fmt::print("{}\n\n", app(method.get(), function.get(), Area::cube(2, -5, 5)));
    //     }
    // }

    // fmt::println("{}\n\n", app());

    return 0;
}
