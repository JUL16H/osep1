#pragma once
#include <stslog/stslog.hpp>
#include <vector>
#include <algorithm>
#include <queue>
#include "Proc.hpp"

class ProcSchedulerBase {
public:
    ProcSchedulerBase(std::vector<Proc> _procs) : procs(_procs) {
        logger = stslog::LogRegistry::instance().get_logger("logger");
    }
    ~ProcSchedulerBase() = default;
    std::vector<unsigned> operator()() { return this->exec(); }

protected:
    std::vector<unsigned> exec() {
        logger->info("使用{}.", this->method_name());
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

        logger->info("{}运行完成, 平均周转时间{:.2f}, 平均带权周转时间{:.2f}.",
            this->method_name(),
            avg_turnaround_time,
            avg_weighted_turnaround_time
        );

        return exec_order;
    }

    virtual void run() = 0;
    virtual constexpr const char* method_name() = 0;

    void exec_process(Proc& p) {
        exec_order.push_back(p.ID);
        logger->debug("时刻{:4d}, 执行Proc{}, 任务到来时间{:2d}, 优先级{}, 用时{:3d}, 结束时间{}.",
            cur_time, p.ID, p.arrival_time, p.priority, p.time_cost, cur_time + p.time_cost);
        cur_time += p.time_cost;
        p.exec_time = p.time_cost;
        p.finish_time = cur_time;
    }

    void exec_process(Proc& p, unsigned t) {
        exec_order.push_back(p.ID);
        t = std::min(t, p.time_cost - p.exec_time);
        logger->debug("时刻{:4d}, 执行Proc{}, 任务到来时间{:2d}, 优先级{}, 用时{:3d}, 仍需{:3d}, 结束时间{}.",
            cur_time, p.ID, p.arrival_time, p.priority, t, p.time_cost - p.exec_time - t, cur_time + t);
        cur_time += t;
        p.exec_time += t;
        if (p.exec_time == p.time_cost)
            p.finish_time = cur_time;
    }

protected:
    std::shared_ptr<stslog::Logger> logger;
    std::vector<unsigned> exec_order;
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
    constexpr const char* method_name() override { return "FCFS"; }
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
    constexpr const char* method_name() override { return "SJF"; }
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
            exec_process(*arrived_procs.top());
            arrived_procs.pop();
        }
    }
    constexpr const char* method_name() override { return "HPF"; }
private:
    struct cmp{
        bool operator()(const Proc* a, const Proc* b)
        { return a->priority > b->priority || a->priority == b->priority && a->arrival_time > b->arrival_time; }
    };
    unsigned idx = 0;
    std::priority_queue<Proc*, std::vector<Proc*>, cmp> arrived_procs;
};

class ProcScheduler_RR : public ProcSchedulerBase {
public:
    ProcScheduler_RR(std::vector<Proc> _procs) : ProcSchedulerBase(_procs) {}
protected:
    void run() override {
        while (true) {
            if (que.empty()) {
                while (idx < procs.size() && cur_time >= procs[idx].arrival_time)
                    que.push(&procs[idx++]);
                if (que.empty()) {
                    if (idx == procs.size())
                        return;
                    cur_time = procs[idx].arrival_time;
                    continue;
                }
            }
            else {
                exec_process(*que.front(), clock_time());
                while (idx < procs.size() && cur_time >= procs[idx].arrival_time)
                    que.push(&procs[idx++]);
                if (que.front()->exec_time < que.front()->time_cost)
                    que.push(que.front());
                que.pop();
            }
        }
    }
    constexpr const char* method_name() override { return "RR"; }
private:
    unsigned idx = 0;
    std::queue<Proc*> que;
    constexpr const unsigned clock_time() { return 100; }
};

class ProcScheduler_MLFQ : public ProcSchedulerBase {
public:
    ProcScheduler_MLFQ(std::vector<Proc> _procs) : ProcSchedulerBase(_procs)
    {
        ques.resize(num_queue());
    }
protected:
    void run() override {
        while (true) {
            while (idx < procs.size() && cur_time >= procs[idx].arrival_time) {
                procs[idx].priority = 0;
                ques[0].push(&procs[idx++]);
            }
            if (std::ranges::all_of(ques, [](std::queue<Proc*> &que){return que.empty(); })) {
                if (idx == procs.size())
                    return;
                cur_time = procs[idx].arrival_time;
                continue;
            }
            for (unsigned i = 0; i < num_queue(); i++) {
                if (!ques[i].empty()) {
                    exec_process(*ques[i].front(), exec_time(i));
                    if (ques[i].front()->exec_time < ques[i].front()->time_cost) {
                        ques[i].front()->priority = std::min(i + 1, num_queue() - 1);
                        ques[std::min(i + 1, num_queue() - 1)].push(ques[i].front());
                    }
                    ques[i].pop();
                    break;
                }
            }
        }
    }
    constexpr const char* method_name() override { return "MLFQ"; }
private:
    unsigned idx = 0;
    constexpr const unsigned num_queue() { return 5; }
    constexpr const unsigned exec_time(unsigned i) { return 5<<i; }
    std::vector<std::queue<Proc*>> ques;
};
