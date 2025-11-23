#pragma once
#include <random>

class RNG {
public:
    RNG() : gen(std::random_device{}()) {}
    unsigned randu(unsigned l, unsigned r) {
        std::uniform_int_distribution<unsigned> dis(l, r);
        return dis(gen);
    }

private:
    std::mt19937 gen;
};
