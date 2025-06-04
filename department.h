#ifndef SOR_DEPARTMENT_H
#define SOR_DEPARTMENT_H

#include "doctor.h"
#include "safe_queue.h"
#include <vector>
#include <array>
#include <atomic>
#include <chrono>

class Department
{
public:
    /* ---------- konstruktor / lifecycle ---------- */
    Department(DepartmentID id, int doctorCount, std::atomic<bool>& run);
    void join();

    /* ---------- główna logika ---------- */
    void applyAging();          // starzenie kolejki
    void updateActivity();      // wywoływane co 1 s – liczy idle / full-load

    /* ---------- API dla Triage ---------- */
    void addPatient(const Patient& p) { queue_.push(p); }
    size_t queueLen() const           { return queue_.size(); }

    /* ---------- statystyki do dashboardu ---------- */
    std::array<int,4> getQueueCounts() const { return queue_.priorityCounts(); }
    int  getFreeDoctors()  const { return totalDoctors_ - busy.load(); }
    int  getTotalDoctors() const { return totalDoctors_; }
    int  getServed()       const { return served.load(); }

    long getIdleTimeMs() const;          // deklaracja (definicja w .cpp)
    long getFullLoadTimeMs() const;      // deklaracja (definicja w .cpp)

    /* ---------- liczniki współdzielone ---------- */
    std::atomic<int> busy{0}, served{0};

private:
    DepartmentID id_;
    SafeQueue    queue_;
    std::vector<std::unique_ptr<Doctor>> docs_;
    int  totalDoctors_;

    /* zmienne pomocnicze do idle / full-load */
    std::chrono::steady_clock::time_point lastCheck_ = std::chrono::steady_clock::now();
    long idleTimeMs_      = 0;
    long fullLoadTimeMs_  = 0;
};

#endif /* SOR_DEPARTMENT_H */
