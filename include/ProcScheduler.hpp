#pragma once
#include <stslog/stslog.hpp>
#include <vector>
#include "Proc.hpp"

class ProcSchedulerBase {
public:
    ProcSchedulerBase(std::vector<Proc> procs) {
        logger = stslog::LogRegistry::instance().get_logger("logger");
        logger->info("使用{}模拟", this->method_name);
    }
    ~ProcSchedulerBase() {
        logger->info("{}模拟结束", this->method_name);
    }

private:
    constexpr static std::string method_name = "";
    std::shared_ptr<stslog::Logger> logger;
};
