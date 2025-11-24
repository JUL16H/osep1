#include <stslog/stslog.hpp>
#include <stslog/sinks.hpp>
#include "ProcGenerator.hpp"
#include "ProcScheduler.hpp"

int main() {
    auto logger = stslog::make_logger("logger",{
        {"stdoutSink", stslog::make_sink<stslog::Sinks::ColoredStdoutSink>()},
        {"fileSink", stslog::make_sink<stslog::Sinks::FileSink>("log.log")}
    });
    logger->set_level(stslog::LogLevel::TRACE);
    logger->set_pattern("[%H:%M:%S:%f] [%l] %v");
    stslog::LogRegistry::instance().enroll_logger(logger);

    ProcGenerator procGenerator;
    procGenerator.randomize_procs(10);

    ProcScheduler_FCFS {procGenerator.get_procs()}();
    ProcScheduler_SJF{procGenerator.get_procs()}();
    ProcScheduler_HPF{procGenerator.get_procs()}();
    ProcScheduler_RR{procGenerator.get_procs()}();
    ProcScheduler_MLFQ{procGenerator.get_procs()}();

    return 0;
}
