#ifndef SOR_MONITOR_H
#define SOR_MONITOR_H

#include "arrival_dispatcher.h"
#include "triage.h"
#include "department.h"
#include "resource_manager.h"
#include <atomic>
#include <thread>
#include <chrono>
#include <string>

class DashboardMonitor
{
    ArrivalDispatcher&                         arrival_;
    Triage&                                    triage_;
    Department&                                surg_;
    Department&                                ortho_;
    Department&                                cardio_;
    ResourceManager&                           rm_;
    std::atomic<bool>&                         running_;

    std::thread                                th_;
    std::chrono::steady_clock::time_point      start_;

    void loop();
    static std::string hhmmss(std::chrono::seconds s);

public:
    DashboardMonitor(ArrivalDispatcher& a,
                     Triage& t,
                     Department& s,
                     Department& o,
                     Department& c,
                     ResourceManager& rm,
                     std::atomic<bool>& run);
    void join();
};

/* alias, by nadal móc pisać Monitor */
using Monitor = DashboardMonitor;

#endif /* SOR_MONITOR_H */
