#ifndef PARSE_H
#define PARSE_H

#include <string>
#include <functional>
#include <sstream>

#include <fmt/format.h>

template <typename T>
bool non_negative(T val) { return val >= 0; }

template <typename T>
bool positive(T val) { return val > 0; }


template <typename T, typename ExtrapolatedT>
std::pair<T, bool>
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
std::pair<T, bool>
parse_int(const std::string& from,
          const std::function<bool(std::int64_t val)>& validate = [](std::int64_t val) -> bool { return true; }) {
    return parse<T, std::int64_t>(from, validate);
}

inline std::int64_t
must_int64(const std::string& from, const bool positive) {
    std::function<bool(std::int64_t val)> validate = [](std::int64_t val) -> bool { return true; };
    if (positive) {
        validate = [](std::int64_t val) -> bool { return val > 0; };
    }

    auto [ret, ok] = parse_int<int64_t>(from, validate);
    if (!ok) {
        throw std::invalid_argument(fmt::format("couldn't parse integer from '{}'", from));
    }
    return ret;
}


inline std::pair<double, bool>
parse_double(const std::string& from,
             const std::function<bool(double val)>& validate = [](double val) -> bool { return true; }) {
    return parse<double, double>(from, validate);
}

inline double
must_double(const std::string& from,
            const std::function<bool(double val)>& validate = [](double val) -> bool { return true; }) {
    auto [ret, ok] = parse_double(from, validate);
    if (!ok) {
        throw std::invalid_argument(fmt::format("couldn't parse double from '{}'", from));
    }
    return ret;
}


#endif //PARSE_H
