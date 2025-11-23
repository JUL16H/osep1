#pragma once
#include <algorithm>
#include "Proc.hpp"
#include "RNG.hpp"

class ProcGenerator {
public:
    void randomize_procs(unsigned n) {
        unsigned cur_id = 0;
        procs.resize(n);
        std::ranges::for_each(procs, [&](Proc &p){
            p = {
                .ID = cur_id++,
                .beg_time = rng.randu(0, 100),
                .time_cost = rng.randu(0, 100)
            };
        });
        std::ranges::sort(procs, [](const Proc &p1, const Proc &p2){ return p1.beg_time < p2.beg_time; });
    }

    auto get_procs() {
        return this->procs;
    }

private:
    std::vector<Proc> procs;
    RNG rng;
};
