#ifndef SOR_ARRIVAL_DISPATCHER_H
#define SOR_ARRIVAL_DISPATCHER_H

#include "triage.h"
#include "config.h"
#include <thread>
#include <atomic>
#include <map>

class ArrivalDispatcher {
public:
    ArrivalDispatcher(Triage& tr, const Config& cfg, std::atomic<bool>& run);

    void join();
    int arrived() const { return totalArrived_; }

    const std::map<long, int>& getArrivalTimeline() const;

private:
    void loop();

    Triage& triage_;
    const Config& cfg_;
    std::atomic<bool>& running_;
    std::thread th_;
    std::atomic<int> totalArrived_{0};

    std::chrono::steady_clock::time_point start_;
    std::map<long, int> arrivalTimeline_;
};

#endif // SOR_ARRIVAL_DISPATCHER_H
