#include "doctor.h"
#include <random>
#include <chrono>

Doctor::Doctor(int id, DepartmentID dept,
               SafeQueue& queue,
               std::atomic<int>& busyCounter,
               std::atomic<int>& servedCounter,
               std::atomic<bool>& run)
        : id_(id), dept_(dept),
          queue_(queue), running_(run),
          busy_(busyCounter), served_(servedCounter)
{
    thread_ = std::thread(&Doctor::work, this);
}

void Doctor::join() { if (thread_.joinable()) thread_.join(); }

/* ─────────────────────────────────────────── */
void Doctor::work()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> contactTime(2, 5);
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    while (running_)
    {
        Patient p;
        if (!queue_.pop(p, running_)) break;

        ++busy_;
        std::this_thread::sleep_for(std::chrono::seconds(contactTime(gen)));

        bool needsSurgery = (prob(gen) < 0.20);
        if (!needsSurgery)
        {
            /* badania obrazowe */
            std::string res = (dept_ == DepartmentID::SURGERY)    ? "CT"  :
                              (dept_ == DepartmentID::ORTHOPEDIC) ? "XRAY":
                              "USG";
            gResMgr->acquire(res);

            int tMin = (res == "CT" ? 8 : (res == "XRAY" ? 3 : 3));
            int tMax = (res == "CT" ?15 : (res == "XRAY" ? 6 : 5));
            std::uniform_int_distribution<int> diag(tMin, tMax);
            std::this_thread::sleep_for(std::chrono::seconds(diag(gen)));

            gResMgr->release(res);
        }
        else
        {
            /* zabieg operacyjny */
            gResMgr->acquire("ANEST");
            gResMgr->acquire("OR");

            std::uniform_int_distribution<int> opTime(10, 20);
            std::this_thread::sleep_for(std::chrono::seconds(opTime(gen)));

            gResMgr->release("OR");
            gResMgr->release("ANEST");

            if (prob(gen) < 0.50) {
                gResMgr->acquire("ICU");
                std::this_thread::sleep_for(std::chrono::seconds(30));
                gResMgr->release("ICU");
            }
        }

        ++served_;
        --busy_;
    }
}
