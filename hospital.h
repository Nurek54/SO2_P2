#ifndef SOR_HOSPITAL_H
#define SOR_HOSPITAL_H

#include <atomic>
#include <thread>
#include <csignal>
#include "config.h"
#include "resource_manager.h"
#include "arrival_dispatcher.h"
#include "triage.h"
#include "department.h"
#include "deadlock_detector.h"
#include "metrics_collector.h"
#include "monitor.h"

class Hospital
{
    std::atomic<bool> running_{true};
    Config            cfg_;

    /* moduły */
    ResourceManager   resMgr_;
    Department        surg_, ortho_, cardio_;
    Triage            triage_;
    ArrivalDispatcher arrival_;
    DeadlockDetector  deadlock_;
    MetricsCollector  metrics_;
    DashboardMonitor  monitor_;

    static Hospital*  gInstance_;

public:
    explicit Hospital(const Config& cfg);
    void run();
    void stop() { running_ = false; }

private:
    static void onSignal(int);
};

#endif /* SOR_HOSPITAL_H */
