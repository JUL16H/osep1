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

    ProcScheduler_FCFS fcfs(procGenerator.get_procs());
    fcfs.exec();

    ProcScheduler_SJF sjf(procGenerator.get_procs());
    sjf.exec();

    ProcScheduler_HPF hpf(procGenerator.get_procs());
    hpf.exec();

    ProcScheduler_RR rr(procGenerator.get_procs());
    rr.exec();

    return 0;
}
