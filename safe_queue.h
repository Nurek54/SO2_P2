#ifndef SOR_SAFE_QUEUE_H
#define SOR_SAFE_QUEUE_H

#include "patient.h"
#include <deque>
#include <mutex>
#include <condition_variable>
#include <array>
#include <atomic>

class SafeQueue {
public:
    void push(const Patient& p);
    bool pop(Patient& out, std::atomic<bool>& running);

    size_t size() const;
    std::array<int, 4> priorityCounts() const;

    // Dla starzenia
    bool peekFront(int prio, Patient& out) const;
    void promoteFront(int fromPrio);
    void applyAging(const std::chrono::seconds threshold[4],
                    const std::chrono::steady_clock::time_point& now);

private:
    mutable std::mutex mtx;
    std::condition_variable cv;
    std::array<std::deque<Patient>, 4> queues; // RED=0 .. BLUE=3
};

#endif // SOR_SAFE_QUEUE_H
