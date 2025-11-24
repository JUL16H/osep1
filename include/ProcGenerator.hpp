#pragma once
#include <stslog/stslog.hpp>
#include <algorithm>
#include "Proc.hpp"
#include "RNG.hpp"

class ProcGenerator {
public:
    ProcGenerator() : logger(stslog::LogRegistry::instance().get_logger("logger")) {
        logger->debug("RNG种子: {}.", rng.get_seed());
    }

    void randomize_procs(unsigned n) {
        logger->info("随机化进程列表, 长度{}.", n);
        unsigned cur_id = 0;
        procs.resize(n);
        std::ranges::for_each(procs, [&](Proc &p){
            p = {
                .ID = cur_id++,
                .arrival_time = rng.randu(0, 100),
                .time_cost = rng.randu(1, 300),
                .priority = rng.randu(1, 5),
            };
        });
        std::ranges::sort(procs, [](const Proc &p1, const Proc &p2){ return p1.arrival_time < p2.arrival_time; });

        logger->debug("随机化完成.");
        for (unsigned i = 0; i < n; i++)
            logger->trace("proc{}: ID:{} arrival_time:{} time_cost: {} priority:{}.", i, procs[i].ID, procs[i].arrival_time, procs[i].time_cost, procs[i].priority);
    }

    auto get_procs() const noexcept {
        return this->procs;
    }

private:
    std::shared_ptr<stslog::Logger> logger;
    std::vector<Proc> procs;
    RNG rng;
};
