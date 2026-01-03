#pragma once

struct Proc {
    unsigned ID;
    unsigned arrival_time;
    unsigned finish_time = 0;
    unsigned time_cost;
    unsigned exec_time = 0;
    unsigned priority;
};
