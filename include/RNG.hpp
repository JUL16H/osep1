#pragma once
#include <random>

class RNG {
public:
    RNG() : seed(std::random_device{}()), gen(seed) {}
    RNG(unsigned _seed) : seed(_seed), gen(seed) {}
    unsigned randu(unsigned l, unsigned r)  {
        std::uniform_int_distribution<unsigned> dis(l, r);
        return dis(gen);
    }
    auto get_seed() const noexcept { return seed; }

private:
    const unsigned seed;
    std::mt19937 gen;
};
