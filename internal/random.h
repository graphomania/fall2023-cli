#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class random {
public:
    static double gen(double min, double max);

    static std::mt19937_64& engine();

    static bool with_chance(double chance);
};

#endif //RANDOM_H
