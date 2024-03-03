#ifndef METHOD_H
#define METHOD_H

#include "log.h"
#include "function.h"


class Method {
protected:
    Log log_;
    mutable std::size_t steps_ = 0;

public:
    virtual ~Method() = default;

    explicit Method(const Log& logger) : log_(logger) {}

    [[nodiscard]] virtual std::string name() const = 0;

    [[nodiscard]] virtual std::size_t steps_took() const { return steps_; };

    virtual Function::Value minimal(Function* func, const Area& where) const = 0;

    virtual std::pair<std::vector<Point>, Function::Value>
    minimal_with_path(Function* func, const Area& where) const = 0;

    virtual Method* log(const Log& new_log) {
        log_ = new_log;
        return this;
    }
};

#endif //METHOD_H
