#ifndef TRACE_H
#define TRACE_H


#include "common.h"

#include <fmt/format.h>

#include <iostream>
#include <chrono>
#include <iomanip>
#include <iostream>


static std::string time_string() {
    const auto t = std::time(nullptr);
    const auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

class Log {
public:
    enum class LEVEL {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        MUTED,
    };

private:
    LEVEL level_;
    std::FILE* to_;
    mutable size_t counter_{};
    mutable bool is_counted_{};
    mutable std::string prefix_;

public:
    static Log null() {
        return Log(LEVEL::MUTED);
    }

    explicit Log(const LEVEL lvl, std::FILE* to = stderr)
        : level_(lvl), to_(to) {}

    const Log& counted() const {
        is_counted_ = true;
        return *this;
    }

    void log(std::string_view level, std::string_view message) const {
        if (is_counted_) {
            fmt::print(to_, "{} {}:{} {}. {}\n", time_string(), level, prefix_, ++counter_, message);
        } else {
            fmt::print(to_, "{} {}:{} {}\n", time_string(), level, prefix_, message);
        }
    }

    Log with(std::string prefix) const {
        auto cp = *this;
        cp.prefix_ = std::move(prefix);
        return cp;
    }

    void debug(const std::string_view messsage) const {
        if (level_ <= LEVEL::DEBUG)
            log("DEBUG", messsage);
    }

    void info(const std::string_view messsage) const {
        if (level_ <= LEVEL::INFO)
            log("INFO", messsage);
    }

    void warn(const std::string_view messsage) const {
        if (level_ <= LEVEL::WARN)
            log("WARN", messsage);
    }

    void error(const std::string_view messsage) const {
        if (level_ <= LEVEL::ERROR)
            log("ERROR", messsage);
    }
};


#endif //TRACE_H
