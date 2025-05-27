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

/* Triage: pobiera pacjentów, losuje im priorytety wg Config, przekazuje na
    odpowiedni oddział i zbiera statystyki. */
class Triage
{
    Department&            surg_;
    Department&            ortho_;
    Department&            cardio_;

    std::atomic<bool>&     running_;
    std::queue<int>        regQueue_;
    std::mutex             mtx_;
    std::condition_variable cv_;
    std::vector<std::thread> nurses_;

    // rozkład priorytetów z Config
    double pRed_, pYellow_, pGreen_, pBlue_;

    std::atomic<int>       processed_{0};

    void nurseLoop();

public:
    std::atomic<int> busy{0};

    /** @param s, o, c   referencje do oddziałów
        @param run      flaga sterująca zakończeniem symulacji
        @param cfg      konfiguracja z rozkładami priorytetów */
    Triage(Department& s,
           Department& o,
           Department& c,
           std::atomic<bool>& run,
           const Config& cfg);

    /// dodaje nowego pacjenta do kolejki rejestracji
    void newPatient(int id);

    /// długość kolejki rejestracji
    std::size_t regLen();

    /// ile pacjentów już przerobiono przez triage
    int done() const { return processed_.load(); }

    /// ile wątków-pielęgniarek jest uruchomionych
    int nurseCount() const { return int(nurses_.size()); }

    /// czeka na zakończenie wszystkich wątków
    void join();
};

#endif /* SOR_TRIAGE_H */
