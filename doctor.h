#ifndef SOR_DOCTOR_H
#define SOR_DOCTOR_H

#include "safe_queue.h"
#include "types.h"
#include "resource_manager.h"
#include <thread>
#include <atomic>

class Doctor
{
    int id_;
    DepartmentID dept_;
    SafeQueue&   queue_;
    std::atomic<bool>& running_;
    std::atomic<int>&  busy_;
    std::atomic<int>&  served_;
    std::thread        thread_;

public:
    Doctor(int id, DepartmentID dept,
           SafeQueue& queue,
           std::atomic<int>& busyCounter,
           std::atomic<int>& servedCounter,
           std::atomic<bool>& run);

    void join();

private:
    void work();
};

#endif /* SOR_DOCTOR_H */
