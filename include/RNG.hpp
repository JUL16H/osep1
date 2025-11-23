#pragma once
#include <random>

class RNG {
public:
    RNG() : seed(std::random_device{}()), gen(seed) {}
    unsigned randu(unsigned l, unsigned r) {
        std::uniform_int_distribution<unsigned> dis(l, r);
        return dis(gen);
    }
    auto get_seed() { return seed; }

private:
    unsigned seed;
    std::mt19937 gen;
};
