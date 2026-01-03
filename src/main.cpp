#include <stslog/stslog.hpp>
#include <stslog/sinks.hpp>
#include "ProcGenerator.hpp"
#include "ProcScheduler.hpp"

int main() {
    auto logger = stslog::make_logger("logger", {
        {"stdoutSink", stslog::make_sink<stslog::Sinks::ColoredStdoutSink>()},
        {"fileSink", stslog::make_sink<stslog::Sinks::FileSink>("log.log")}
    });
    logger->set_level(stslog::LogLevel::TRACE);
    logger->set_pattern("[%H:%M:%S:%e] [%l] %v");
    stslog::LogRegistry::instance().enroll_logger(logger);

    ProcGenerator procGenerator;
    procGenerator.randomize_procs(10);
    const auto& procs = procGenerator.get_procs();

    ProcScheduler_FCFS{procs}();
    ProcScheduler_SJF{procs}();
    ProcScheduler_HPF{procs}();
    ProcScheduler_RR{procs}();
    ProcScheduler_MLFQ{procs}();

    return 0;
}
