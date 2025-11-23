#pragma once
#include <stslog/stslog.hpp>
#include <vector>
#include "Proc.hpp"
#include <algorithm>
#include <queue>

class ProcSchedulerBase {
public:
    ProcSchedulerBase(std::vector<Proc> _procs) : procs(_procs) {
        logger = stslog::LogRegistry::instance().get_logger("logger");
    }

    void exec() {
        logger->info("使用{}模拟.", this->method_name());
        run();

        double avg_turnaround_time = 0;
        double avg_weighted_turnaround_time = 0;

        std::ranges::for_each(procs, [&](Proc& p){
            unsigned turnaround_time = p.finish_time - p.arrival_time;
            avg_turnaround_time += turnaround_time;
            avg_weighted_turnaround_time += (1.0 * turnaround_time / p.time_cost);
        });

        avg_turnaround_time /= procs.size();
        avg_weighted_turnaround_time /= procs.size();

        logger->info("{}模拟结束, 平均周转时间{:.2f}, 平均带权周转时间{:.2f}.",
            this->method_name(),
            avg_turnaround_time,
            avg_weighted_turnaround_time
        );
    }

protected:
    virtual void run() = 0;
    virtual constexpr char* method_name() = 0;

    void exec_process(Proc& p) {
        logger->debug("模拟时刻{}, 执行Proc{}, 任务到来时间{}, 优先级{}, 需要用时{}, 结束时间{}.", cur_time, p.ID, p.arrival_time, p.priority, p.time_cost, cur_time + p.time_cost);
        cur_time += p.time_cost;
        p.finish_time = cur_time;
    }

protected:
    std::shared_ptr<stslog::Logger> logger;
    unsigned cur_time = 0;
    std::vector<Proc> procs;
};

class ProcScheduler_FCFS : public ProcSchedulerBase {
public:
    ProcScheduler_FCFS(std::vector<Proc> _procs) : ProcSchedulerBase(_procs) {}
protected:
    void run() override {
        for (unsigned i = 0; i < procs.size(); i++) {
            cur_time = std::max(cur_time, procs[i].arrival_time);
            exec_process(procs[i]);
        }
    }
    const char* method_name() override { return "FCFS"; }
};

class ProcScheduler_SJF : public ProcSchedulerBase {
public:
    ProcScheduler_SJF(std::vector<Proc> _procs) : ProcSchedulerBase(_procs) {}
protected:
    void run() override {
        while (true) {
            while (idx < procs.size() && cur_time >= procs[idx].arrival_time)
                arrived_procs.push(&procs[idx++]);
            if (arrived_procs.empty()) {
                if (idx == procs.size())
                    return;
                cur_time = procs[idx].arrival_time;
                continue;
            }
            if (!arrived_procs.empty()) {
                exec_process(*arrived_procs.top());
                arrived_procs.pop();
            }
        }
    }
    const char* method_name() override { return "SJF"; }
private:
    struct cmp{
        bool operator()(const Proc* a, const Proc* b) { return a->time_cost > b->time_cost; }
    };
    unsigned idx = 0;
    std::priority_queue<Proc*, std::vector<Proc*>, cmp> arrived_procs;
};

class ProcScheduler_HPF : public ProcSchedulerBase {
public:
    ProcScheduler_HPF(std::vector<Proc> _procs) : ProcSchedulerBase(_procs) {}
protected:
    void run() override {
        while (true) {
            while (idx < procs.size() && cur_time >= procs[idx].arrival_time)
                arrived_procs.push(&procs[idx++]);
            if (arrived_procs.empty()) {
                if (idx == procs.size())
                    return;
                cur_time = procs[idx].arrival_time;
                continue;
            }
            if (!arrived_procs.empty()) {
                exec_process(*arrived_procs.top());
                arrived_procs.pop();
            }
        }
    }
    const char* method_name() override { return "HPF"; }
private:
    struct cmp{
        bool operator()(const Proc* a, const Proc* b) { return a->priority > b->priority || a->priority == b->priority && a->time_cost > b->time_cost; }
    };
    unsigned idx = 0;
    std::priority_queue<Proc*, std::vector<Proc*>, cmp> arrived_procs;
};
