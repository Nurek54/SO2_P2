#include "hospital.h"
#include <chrono>
#include <iostream>

Hospital* Hospital::gInstance_ = nullptr;

/* ───────────── ctor ───────────── */
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
}

/* ───────────── run ───────────── */
void Hospital::run()
{
    std::cout << "\n=== SYMULACJA START ===  (Ctrl+C aby zatrzymać)\n";

    while (running_)
        std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "\nZatrzymywanie...\n";

    arrival_.join();
    triage_.join();
    surg_.join(); ortho_.join(); cardio_.join();
    deadlock_.join();
    metrics_.join();
    monitor_.join();

    std::cout << "Koniec symulacji.\n";
}

/* ───────────── SIGINT handler ───────────── */
void Hospital::onSignal(int)
{
    if (gInstance_) gInstance_->stop();
}
