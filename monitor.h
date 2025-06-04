#ifndef SOR_MONITOR_H
#define SOR_MONITOR_H

#include <atomic>
#include <chrono>
#include <thread>
#include <string>
#include <map>
#include <vector>
#include "arrival_dispatcher.h"
#include "triage.h"
#include "department.h"
#include "resource_manager.h"

class DashboardMonitor {
    ArrivalDispatcher&        arrival_;
    Triage&                   triage_;
    Department&               surg_;
    Department&               ortho_;
    Department&               cardio_;
    ResourceManager&          rm_;
    std::atomic<bool>&        running_;
    std::chrono::steady_clock::time_point start_;
    std::thread               th_;

public:
    DashboardMonitor(ArrivalDispatcher& a,
                     Triage& t,
                     Department& s,
                     Department& o,
                     Department& c,
                     ResourceManager& rm,
                     std::atomic<bool>& run);

    void join();

private:
    void loop();
    std::string hhmmss(std::chrono::seconds s);
};

#endif // SOR_MONITOR_H
