#ifndef SOR_SAFE_QUEUE_H
#define SOR_SAFE_QUEUE_H

#include "patient.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <array>

struct PatientCmp {
    bool operator()(const Patient& a, const Patient& b) const {
        if (a.priority != b.priority)
            return static_cast<int>(a.priority) > static_cast<int>(b.priority);
        return a.queued > b.queued;
    }
};

class SafeQueue {
    std::priority_queue<Patient, std::vector<Patient>, PatientCmp> pq;
    mutable std::mutex mtx;
    std::condition_variable cv;

public:
    void push(const Patient& p);
    bool pop(Patient& out, std::atomic<bool>& running);
    size_t size() const;

    // liczba pacjentow w kolejce wg priorytetu {R,Y,G,B}
    std::array<int,4> priorityCounts() const;
};

#endif // SOR_SAFE_QUEUE_H
