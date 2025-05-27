#ifndef SOR_DEADLOCK_DETECTOR_H
#define SOR_DEADLOCK_DETECTOR_H

#include "config.h"
#include "resource_manager.h"
#include <thread>
#include <atomic>
#include <unordered_map>
#include <string>

class DeadlockDetector {
public:
    DeadlockDetector(ResourceManager& rm, const Config& cfg,
                     std::atomic<bool>& run);
    void join();

private:
    void loop();
    bool detect();    // wykrycie zakleszczenia
    void resolve();   // reakcja na deadlock

    ResourceManager& rm_;
    const Config& cfg_;
    std::atomic<bool>& running_;
    std::thread th_;

    // historia zasobow do detekcji
    std::unordered_map<std::string, int> lastInUse_;
    std::unordered_map<std::string, int> sameCounter_;
};

#endif // SOR_DEADLOCK_DETECTOR_H
