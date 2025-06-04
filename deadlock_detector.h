#ifndef SOR_DEADLOCK_DETECTOR_H
#define SOR_DEADLOCK_DETECTOR_H

#include <atomic>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include "resource_manager.h"
#include "config.h"

class DeadlockDetector {
public:
    DeadlockDetector(ResourceManager& rm, const Config& cfg, std::atomic<bool>& run);
    void join();

private:
    ResourceManager& rm_;
    const Config& cfg_;
    std::atomic<bool>& running_;
    std::thread th_;

    // Dwa słowniki: ostatnia obserwowana liczba użytych sztuk oraz licznik niezmienności.
    std::unordered_map<std::string, int> lastInUse_;
    std::unordered_map<std::string, int> sameCounter_;

    void loop();
    bool detect();
    void resolve();
};

#endif // SOR_DEADLOCK_DETECTOR_H
