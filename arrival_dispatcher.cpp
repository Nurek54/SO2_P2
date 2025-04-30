#include "arrival_dispatcher.h"
#include <random>
#include <chrono>
#include <iostream>

ArrivalDispatcher::ArrivalDispatcher(Triage& tr, const Config& cfg,
                                     std::atomic<bool>& run)
        : triage_(tr), cfg_(cfg), running_(run),
          start_(std::chrono::steady_clock::now())
{
    th_ = std::thread(&ArrivalDispatcher::loop, this);
}

void ArrivalDispatcher::join() { if (th_.joinable()) th_.join(); }

void ArrivalDispatcher::loop()
{
    std::mt19937 gen(std::random_device{}());
    std::exponential_distribution<double> exp(cfg_.lambdaPerSec);

    int nextId = 1;
    bool massFired = false;

    while (running_)
    {
        /* zwykłe przyjęcia */
        std::this_thread::sleep_for(std::chrono::duration<double>(exp(gen)));
        triage_.newPatient(nextId++);
        ++totalArrived_;

        /* zdarzenie masowe */
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - start_).count();
        if (cfg_.massEnabled && !massFired && elapsed >= cfg_.massAtSec)
        {
            massFired = true;
            int perSec = static_cast<int>(cfg_.massRatePerSec);
            std::cout << "[ArrivalDispatcher] zdarzenie masowe! +" << perSec
                      << " pacjentów/s\n";
            for (int i = 0; i < perSec; ++i) {
                triage_.newPatient(nextId++);
                ++totalArrived_;
            }
        }
    }
}
