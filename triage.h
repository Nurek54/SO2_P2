#ifndef SOR_TRIAGE_H
#define SOR_TRIAGE_H

#include "department.h"
#include "config.h"
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <atomic>

class Triage
{
    Department& surg_;
    Department& ortho_;
    Department& cardio_;

    std::atomic<bool>& running_;
    std::queue<int>    regQueue_;
    std::mutex         mtx_;
    std::condition_variable cv_;
    std::vector<std::thread> nurses_;

    /* rozkład priorytetów */
    double pRed_, pYellow_, pGreen_, pBlue_;

    std::atomic<int> processed_{0};

    void nurseLoop();

public:
    std::atomic<int> busy{0};

    Triage(Department& s, Department& o, Department& c,
           std::atomic<bool>& run, const Config& cfg, int nurseCnt = 2);

    /* API dla ArrivalDispatcher */
    void newPatient(int id);

    /* statystyki */
    std::size_t regLen();
    int  done() const { return processed_.load(); }

    void join();
};

#endif /* SOR_TRIAGE_H */
