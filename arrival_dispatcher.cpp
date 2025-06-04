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

void ArrivalDispatcher::join() {
    if (th_.joinable()) th_.join();
}

const std::map<long, int>& ArrivalDispatcher::getArrivalTimeline() const {
    return arrivalTimeline_;
}

void ArrivalDispatcher::loop()
{
    std::mt19937 gen(std::random_device{}());

    double lambda = (cfg_.lambdaPerSec <= 0.0) ? 0.001 : cfg_.lambdaPerSec;
    std::exponential_distribution<double> exp(lambda);

    int nextId = 1;
    bool massFired = false;

    while (running_)
    {
        if (cfg_.maxPatients >= 0 && totalArrived_ >= cfg_.maxPatients) {
            /*std::cout << "[ArrivalDispatcher] Osiągnięto limit "
                      << cfg_.maxPatients << " pacjentów. Wstrzymano dalszy napływ.\n";*/
            break;
        }

        std::this_thread::sleep_for(std::chrono::duration<double>(exp(gen)));

        triage_.newPatient(nextId++);
        ++totalArrived_;

        long ts = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start_).count();
        arrivalTimeline_[ts / 1000]++;

        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - start_).count();

        if (cfg_.massEnabled && !massFired && elapsed >= cfg_.massAtSec)
        {
            massFired = true;
            int perSec = static_cast<int>(cfg_.massRatePerSec);
            /*std::cout << "[ArrivalDispatcher] zdarzenie masowe! +" << perSec
                      << " pacjentow/s\n";*/

            for (int i = 0; i < perSec; ++i) {
                if (cfg_.maxPatients >= 0 && totalArrived_ >= cfg_.maxPatients) break;
                triage_.newPatient(nextId++);
                ++totalArrived_;

                long tsMass = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start_).count();
                arrivalTimeline_[tsMass / 1000]++;
            }
        }
    }
}
