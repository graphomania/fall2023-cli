#ifndef CLI_H
#define CLI_H


#include "../internal/method.h"
#include "../internal/method_nelder_mead.h"
#include "../internal/method_random_walk.h"

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <functional>
#include <fstream>

#include "parse.h"


std::vector<std::string> stringify(const int argc, char* argv[]) {
    auto ret = std::vector<std::string>(argc);
    for (size_t i = 0; i < argc; i++) {
        ret[i] = argv[i];
    }
    return ret;
}

int cli_app(int argc, char* argv[]);

class CLI {
public:
    template <typename T, typename ExtrapolatedT>
    static std::pair<T, bool>
    parse(const std::string& from,
          const std::function<bool(ExtrapolatedT val)>& validate = [](ExtrapolatedT val) -> bool { return true; }) {
        ExtrapolatedT i;

        std::istringstream iss(from);
        iss >> i;

        if (!validate(i)) {
            return {T{}, false};
        }

        return {static_cast<T>(i), true};
    }


    template <typename T>
    static std::pair<T, bool>
    parse_int(const std::string& from,
              const std::function<bool(std::int64_t val)>& validate = [](std::int64_t val) -> bool { return true; }) {
        return CLI::parse<T, std::int64_t>(from, validate);
    }

    static std::pair<double, bool>
    parse_double(const std::string& from,
                 const std::function<bool(double val)>& validate = [](double val) -> bool { return true; }) {
        return CLI::parse<double, double>(from, validate);
    }

    static bool positive(const std::int64_t val) { return val > 0; }

    class Argument {
    public:
        using OnParse = void(CLI& cli, std::vector<std::string> args);

        std::vector<std::string> alisases;
        std::function<OnParse> on_parse;
        size_t argc;
        std::vector<std::string> args;
        std::string help;

        /*implicit*/
        Argument(OnParse on_parse, const std::vector<std::string>& alisases_, size_t argc = 1, std::string help = "")
            : alisases(alisases_), on_parse(on_parse), argc(argc) {
            this->help = std::move(help);
        }
    };

private:
    std::vector<CLI::Argument> allowed_arguments_;
    std::optional<CLI::Argument*> caputured_arg{};

    std::optional<CLI::Argument*> get_arg(const std::string_view arg_str) {
        for (auto& arg : allowed_arguments_) {
            for (const auto& alias : arg.alisases) {
                if (alias == arg_str) {
                    return {&arg};
                }
            }
        }

        return {};
    }

public:
    std::vector<std::shared_ptr<Method>> methods;
    std::vector<std::shared_ptr<Function>> functions;
    Area area{{{-5, -5}}, {{5, 5}}};

    explicit CLI(const std::vector<Argument>& args) : allowed_arguments_(args) {}

    [[nodiscard]]
    std::vector<CLI::Argument> allowed_arguments() const {
        return allowed_arguments_;
    }

    void parse(const std::vector<std::string>& args) {
        size_t i = 0;
        for (const auto& arg_str : args) {
            // ignore the program's name
            if (i++ == 0) { continue; }

            if (caputured_arg.has_value()) {
                caputured_arg.value()->args.push_back(arg_str);
                if (caputured_arg.value()->args.size() == caputured_arg.value()->argc) {
                    caputured_arg.value()->on_parse(*this, std::move(caputured_arg.value()->args));
                    caputured_arg = {};
                }
                continue;
            }

            auto arg = get_arg(arg_str);
            if (!arg.has_value()) {
                throw std::invalid_argument(fmt::format("unknown argument '{}'", arg_str));
            }

            arg.value()->args.push_back(arg_str);
            if (arg.value()->args.size() < arg.value()->argc) {
                caputured_arg = {arg.value()};
                continue;
            }
            arg.value()->on_parse(*this, std::move(arg.value()->args));
        }
        if (caputured_arg.has_value()) {
            throw std::invalid_argument(fmt::format("argument '{}' requires more info",
                                                    caputured_arg.value()->args[0]));
        }
    }

    int operator()() {
        try {
            for (auto func : functions) {
                for (auto method : methods) {
                    auto [min, min_val] = method->minimal(func.get(), area);

                    auto [closest, closest_val] = func->closest_minimal(min);
                    fmt::print("Function: {} in {} | Method: {}\n"
                               "\tResults in minimum at x={}, f(x)={} (in {} steps).\n"
                               "\tThe closest theoretically known local minimum: y={}, f(y)={}\n"
                               "\t||(x, f(x)) - (y, f(y))|| = {}))\n",
                               func->name(), area.to_string(), method->name(),
                               min, min_val, method->steps_took(),
                               closest, closest_val,
                               min.dist_with(closest, [func](const auto& p) { return (*func)(p); })
                    );
                }
                fmt::print("\n");
            }
        } catch (std::invalid_argument& e) {
            fmt::print(stderr, "got an error, exiting...\n\n{}", e.what());
            return 1;
        }

        return 0;
    }
};

/// this is a joke. not a funny one, but I've cracked a smile. warning: this is a reserved identifier.
/// std::ostringstream would be more optimal, but this is not a bottleneck for sure.
/// std::views::join source code is lovely, just look at it for a moment. tabulation with negative 2 spaces is the cherry on top.
std::string __________________________join(const std::vector<std::string>& vec, const std::string_view sep) {
    std::string ret;
    for (size_t i = 0; i < vec.size(); i++) {
        if (i != 0) {
            ret += sep;
        }
        ret += vec[i];
    }
    return ret;
}

/// clearly copypasted
std::vector<std::string> customSplit(const std::string& str, const char sep) {
    std::vector<std::string> ret;
    int startIndex = 0, endIndex = 0;
    for (int i = 0; i <= str.size(); i++) {
        if (str[i] == sep || i == str.size()) {
            endIndex = i;
            std::string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            ret.push_back(temp);
            startIndex = endIndex + 1;
        }
    }
    return ret;
}

template <typename T, typename Func>
std::vector<T> _____map(std::vector<T> what, Func func) {
    for (size_t i = 0; i < what.size(); i++) {
        what[i] = func(what[i]);
    }
    return what; // RAII, i hope
}

std::string ________left_pad_single(const std::string& what, const size_t size,
                                    const char filler = ' ', const char sep = '\n') {
    if (what.size() > size) {
        return what + filler;
    }
    return what + std::string(size - what.size(), filler);
}

std::string ________left_pad(const std::string& what, const size_t size) {
    return __________________________join(
        _____map(
            customSplit(what, '\n'),
            [size](auto val) -> auto { return ________left_pad_single(val, size); }
        ), "\n");
}

int cli_app(int argc, char* argv[]) {
    auto cli = CLI{
        {
            // Nelder-Mead method
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    auto muted = Log(Log::LEVEL::MUTED);
                    cli.methods.emplace_back(std::make_shared<NelderMead>(muted));
                },
                {"-N", "--nelder", "--nelder-mead"}, 1,
                fmt::format("METHOD: use {}", NelderMead(Log::null()).name())
            },
            // RandomWalk method
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    auto muted = Log(Log::LEVEL::MUTED);
                    cli.methods.emplace_back(std::make_shared<RandomWalk>(muted));
                },
                {"-W", "--walk", "--random-walk"}, 1,
                fmt::format("METHOD: use {}", RandomWalk(Log::null()).name())
            },

            // Himmelblau function
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    auto muted = Log(Log::LEVEL::MUTED);
                    cli.functions.emplace_back(std::make_shared<HimmelblauFunction>());
                },
                {"-H", "--himmelblau"}, 1,
                fmt::format("FUNCTION: {}, NOTE: usable only in R^2.\n"
                            "                                    Has {} known local minimals: {}.\n"
                            "                                    WIKI: https://en.wikipedia.org/wiki/Himmelblau%27s_function",
                            HimmelblauFunction().name(),
                            HimmelblauFunction().minimal().size(),
                            HimmelblauFunction().minimal()
                )
            },
            // Rastrigin function
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    auto size = 2;
                    if (args.size() > 1) {
                        auto [size_, valid] = CLI::parse_int<std::size_t>(args[1], CLI::positive);
                        if (!valid) {
                            throw std::invalid_argument(fmt::format("{} has to be > 1 (got {})", args[0], args[1]));
                        }
                        size = size_;
                    }
                    cli.functions.emplace_back(std::make_shared<RastriginFunction>(size));
                },
                {"-R", "--rastrigin"}, 2,
                fmt::format(
                    "FUNCTION: {}, REQUIRES: <N> -- additional dimension size hint.\n"
                    "                                    Has {} known local minimal: {}.\n"
                    "                                    WIKI: https://en.wikipedia.org/wiki/Rastrigin_function",
                    RastriginFunction(3).name(),
                    RastriginFunction(3).minimal().size(),
                    RastriginFunction(3).minimal()
                )
            },

            // Area
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    size_t dimensions = cli.area.dimensions();
                    double min = -5;
                    double max = 5;

                    if (args.size() > 1) {
                        auto [size_, valid] = CLI::parse_int<std::size_t>(args[1], CLI::positive);
                        if (!valid) {
                            throw std::invalid_argument(fmt::format("{} has to be > 1 (got {})", args[0], args[1]));
                        }
                        dimensions = size_;
                    } else if (args.size() > 2) {
                        auto [min_, valid] = CLI::parse_double(args[1]);
                        if (!valid) {
                            throw std::invalid_argument(fmt::format("{} has to be > 1 (got {})", args[0], args[1]));
                        }
                        min = min_;
                    } else if (args.size() > 3) {
                        auto [max_, valid] = CLI::parse_double(args[1]);
                        if (!valid) {
                            throw std::invalid_argument(fmt::format("{} has to be > 1 (got {})", args[0], args[1]));
                        }
                        max = max_;
                    }
                    cli.area = Area::cube(dimensions, min, max);
                },
                {"-a", "--area"}, 4,
                "cubic area info, REQUIRES: subarguments <DIMENSIONS> <MINIMUM> <MAXIMUM> (default: [-5, 5]x[-5, 5])"
            },

            // Area custom
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    std::string filename = args[1];
                    std::ifstream fin(filename);
                    size_t dimensions;
                    fin >> dimensions;
                    std::vector<double> min(dimensions), max(dimensions);
                    for (size_t i = 0; i < dimensions; i++) {
                        fin >> min[i] >> max[i];
                    }

                    cli.area = Area({min}, {max});
                },
                {"-ac", "--area-custom"}, 2,
                R"(read custom area bounds from subargument <FILE>, which has to be formatted as '<DIMENSIONS>\n<MIN> <MAX>\n<MIN> <MAX>\n...')"
            },

            // Seed
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    auto [seed, valid] = CLI::parse_int<std::size_t>(args[1]);
                    random::engine() = std::mt19937_64(seed);
                },
                {"-s", "--seed"}, 2,
                "seed for the random number generator"
            },

            // Help
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    std::string message;

                    std::vector<std::string> arguments_help;
                    size_t max_aliases_len = 0;
                    for (const auto& arg : cli.allowed_arguments()) {
                        max_aliases_len = std::max(__________________________join(arg.alisases, ", ").size(),
                                                   max_aliases_len);
                    }

                    for (const auto& arg : cli.allowed_arguments()) {
                        arguments_help.push_back(fmt::format("  {}  ---  {}",
                                                             ________left_pad(
                                                                 __________________________join(arg.alisases, ", "),
                                                                 max_aliases_len), arg.help

                        ));
                    }

                    fmt::print("Usage: ./fall2023 <METHODS> <FUCNTIONS> <OPTIONS>\n"
                               "Arguments:\n"
                               "{}\n", __________________________join(arguments_help, "\n"));

                    exit(0);
                },
                {"-h", "--help"}, 1,
                "print this message and exit"
            },

            // Trace
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    for (const auto& method : cli.methods) {
                        method->log(Log(Log::LEVEL::DEBUG));
                    }
                },
                {"-t", "--trace"}, 1,
                "print tracing info (like steps in methods)",
            },

            // Starting point in R^2
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    for (auto& method : cli.methods) {
                        method->with_start(Point{
                            {
                                parse_double(args[1]).first,
                                parse_double(args[2]).first
                            }
                        });
                    }
                },
                {"-sp2", "--starting-point-2"}, 3,
                "starting point (works in R^2 only), usage: -sp <X> <Y>",
            },

            // Starting point in R^3
            CLI::Argument{
                [](CLI& cli, std::vector<std::string> args) {
                    for (auto& method : cli.methods) {
                        method->with_start(Point{
                            {
                                must_double(args[1]),
                                must_double(args[2]),
                                must_double(args[3])
                            }
                        });
                    }
                },
                {"-sp3", "--starting-point-3"}, 4,
                "starting point (works in R^3 only), usage: -sp <X> <Y> <Z>",
            },
        }
    };

    const auto args = stringify(argc, argv);
    cli.parse(args);
    return cli();
}

#endif //CLI_H
