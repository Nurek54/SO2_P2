#include "deadlock_detector.h"
#include <chrono>
#include <iostream>

DeadlockDetector::DeadlockDetector(ResourceManager& rm, const Config& cfg,
                                   std::atomic<bool>& run)
        : rm_(rm), cfg_(cfg), running_(run)
{
    th_ = std::thread(&DeadlockDetector::loop, this);
}

void DeadlockDetector::join()
{
    if (th_.joinable()) th_.join();
}

void DeadlockDetector::loop()
{
    using namespace std::chrono;
    while (running_)
    {
        std::this_thread::sleep_for(
                milliseconds(cfg_.backoffMinMs));

        if (detect())
        {
            std::cout << "[DeadlockDetector] dead-lock!\n";
            resolve();
            std::this_thread::sleep_for(
                    milliseconds(cfg_.backoffMaxMs));
        }
    }
}

/* --- STUBy --- */
bool DeadlockDetector::detect()    { return false; }
void DeadlockDetector::resolve()   {/* TODO */}
