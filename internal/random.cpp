#include "random.h"
#include <random>

double random::gen(const double min, const double max) {
    return std::uniform_real_distribution(min, max)(random::engine());
}

std::mt19937_64& random::engine() {
    static std::mt19937_64 engine_(1);
    return engine_;
}

bool random::with_chance(const double chance) {
    return gen(0, 1) < chance;
}
