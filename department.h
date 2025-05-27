#ifndef SOR_DEPARTMENT_H
#define SOR_DEPARTMENT_H

#include "doctor.h"
#include "safe_queue.h"
#include <vector>
#include <array>
#include <atomic>

class Department
{
    DepartmentID id_;
    SafeQueue    queue_;
    std::vector<std::unique_ptr<Doctor>> docs_;
    int  totalDoctors_;

public:
    std::atomic<int> busy{0}, served{0};

    Department(DepartmentID id, int doctorCount, std::atomic<bool>& run);

    /* API dla Triage */
    void addPatient(const Patient& p) { queue_.push(p); }
    size_t queueLen() const           { return queue_.size(); }

    /* statystyki do dashboardu */
    std::array<int,4> getQueueCounts() const { return queue_.priorityCounts(); }
    int  getFreeDoctors()  const { return totalDoctors_ - busy.load(); }
    int  getTotalDoctors() const { return totalDoctors_; }
    int  getServed()       const { return served.load(); }

    void join();

    void applyAging();

};

#endif /* SOR_DEPARTMENT_H */
