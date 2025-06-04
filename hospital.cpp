#include "hospital.h"
#include "patient_logger.h"
#include <chrono>
#include <iostream>
#include <csignal>
#include <limits> // dla std::numeric_limits

Hospital* Hospital::gInstance_ = nullptr;

Hospital::Hospital(const Config& cfg)
        : cfg_(cfg),
          resMgr_(cfg_),
          surg_ (DepartmentID::SURGERY   , cfg_.chirurg  , running_),
          ortho_(DepartmentID::ORTHOPEDIC, cfg_.ortopeda , running_),
          cardio_(DepartmentID::CARDIO   , cfg_.kardiolog, running_),
          triage_(surg_, ortho_, cardio_, running_, cfg_),
          arrival_(triage_, cfg_, running_),
          deadlock_(resMgr_, cfg_, running_),
          metrics_(running_, cfg_, surg_, ortho_, cardio_, resMgr_),
          monitor_(arrival_, triage_, surg_, ortho_, cardio_, resMgr_, running_)
{
    gInstance_ = this;
    std::signal(SIGINT, &Hospital::onSignal);

    gPatientLogger.start();
}

void Hospital::run()
{
    std::cout << "\n=== SYMULACJA START ===  (Ctrl+C aby zatrzymać)\n";

    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        if (cfg_.maxPatients > 0) {
            // ✅ poprawka: zliczaj tylko pacjentów obsłużonych przez lekarzy
            int totalHandled = surg_.getServed()
                               + ortho_.getServed()
                               + cardio_.getServed();

            if (totalHandled >= cfg_.maxPatients) {
                std::cout << "[Hospital] Wszyscy pacjenci (" << totalHandled
                          << "/" << cfg_.maxPatients << ") zostali obsłużeni.\n";
                running_ = false;
            }
        }
    }

    std::cout << "\nZatrzymywanie...\n";

    arrival_.join();
    triage_.join();
    surg_.join();
    ortho_.join();
    cardio_.join();
    deadlock_.join();
    metrics_.join();
    monitor_.join();

    gPatientLogger.stop();
    gPatientLogger.summary(std::cout);

    std::cout << "Koniec symulacji.\n";

    std::cout << "\nWciśnij ENTER, aby zakończyć...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void Hospital::onSignal(int)
{
    if (gInstance_)
        gInstance_->stop();
}
