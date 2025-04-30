#ifndef SOR_DEADLOCK_DETECTOR_H
#define SOR_DEADLOCK_DETECTOR_H

#include "resource_manager.h"
#include "config.h"
#include <thread>
#include <atomic>

class DeadlockDetector
{
    ResourceManager& rm_;
    const Config&    cfg_;
    std::atomic<bool>& running_;
    std::thread      th_;

public:
    DeadlockDetector(ResourceManager& rm, const Config& cfg,
                     std::atomic<bool>& run);

    void join();

private:
    void loop();
    bool detect();
    void resolve();
};

#endif /* SOR_DEADLOCK_DETECTOR_H */
