#include "monitor.h"
#include <iostream>
#include <iomanip>
#include "patient_logger.h"

#define CLR_RESET   "\033[0m"
#define CLR_YELLOW  "\033[1;33m"
#define CLR_GREEN   "\033[1;32m"
#define CLR_RED     "\033[1;31m"
#define CLR_WHITE   "\033[1;37m"

DashboardMonitor::DashboardMonitor(ArrivalDispatcher& a,
                                   Triage& t,
                                   Department& s,
                                   Department& o,
                                   Department& c,
                                   ResourceManager& rm,
                                   std::atomic<bool>& run)
        : arrival_(a)
        , triage_(t)
        , surg_(s)
        , ortho_(o)
        , cardio_(c)
        , rm_(rm)
        , running_(run)
        , start_(std::chrono::steady_clock::now())
{
    th_ = std::thread(&DashboardMonitor::loop, this);
}

void DashboardMonitor::join() {
    if (th_.joinable()) th_.join();
}

void DashboardMonitor::loop() {
    using namespace std::chrono_literals;

    while (running_) {

        surg_.applyAging();
        ortho_.applyAging();
        cardio_.applyAging();

        std::this_thread::sleep_for(1s);

        auto now     = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_);
        std::string simTime = hhmmss(elapsed);

        int active = triage_.regLen() + triage_.busy
                     + surg_.queueLen() + ortho_.queueLen() + cardio_.queueLen()
                     + surg_.busy     + ortho_.busy      + cardio_.busy;

        /* ---------- ekran ---------- */
#ifdef _WIN32
        system("cls");
#else
        std::cout << "\033[2J\033[H" << std::flush;
#endif
        std::cout << CLR_YELLOW
                  << "==================== S O R    D A S H B O A R D ====================\n"
                  << "Czas symulacji: " << simTime
                  << "   Aktywni pacjenci: " << std::setw(4) << active << "\n\n";

        /* ---------- Wejście + triage ---------- */
        std::cout << CLR_GREEN
                  << "[Wejscie] przyjeto: " << arrival_.arrived()
                  << "   w kolejce do triage: " << triage_.regLen() << '\n'
                  << "[Triage]  pielegniarki zajete: "
                  << ((triage_.busy >= triage_.nurseCount()) ? CLR_RED : CLR_WHITE)
                  << triage_.busy << CLR_WHITE
                  << "/" << triage_.nurseCount()
                  << "   obsluzono: " << triage_.done() << "\n\n";

        /* ---------- Oddziały ---------- */
        std::cout << CLR_GREEN
                  << "Oddzial      |  R |  Y |  G |  B | Wolni/Lek | Obsluzeni |\n"
                  << "-------------|----|----|----|----|------------|------------|\n";
        auto deptLine = [&](Department& d, const char* name){
            auto cnt  = d.getQueueCounts();
            int freeD = d.getFreeDoctors();
            int totD  = d.getTotalDoctors();
            std::cout << std::left << std::setw(13) << name << "|"
                      << std::right
                      << std::setw(3) << cnt[0] << " |"
                      << std::setw(3) << cnt[1] << " |"
                      << std::setw(3) << cnt[2] << " |"
                      << std::setw(3) << cnt[3] << " | "
                      << ((freeD==0)?CLR_RED:CLR_WHITE)
                      << std::setw(3) << freeD << "/" << std::setw(3) << totD
                      << CLR_WHITE << "   | "
                      << std::setw(10) << d.getServed() << " |\n";
        };
        deptLine(surg_,  "Chirurgia");
        deptLine(ortho_, "Ortopedia");
        deptLine(cardio_,"Kardiologia");
        std::cout << '\n';

        /* ---------- Zasoby ---------- */
        std::cout << CLR_GREEN
                  << "Zasob               | Zajete/Wszystkie | % uzycia (rolling 60 s) |\n"
                  << "--------------------|------------------|--------------------------|\n";
        auto resLine = [&](const char* label, const char* key){
            int used  = rm_.inUse(key);
            int total = rm_.total(key);
            int util  = int(rm_.getUtilization(key)*100 + 0.5);
            std::cout << std::left << std::setw(20) << label << "| "
                      << ((used==total)?CLR_RED:CLR_WHITE)
                      << std::right << std::setw(4) << used << "/"
                      << std::setw(3) << total << "         "
                      << CLR_WHITE << "| "
                      << std::setw(3) << util << " %"
                      << "                      |\n";
        };

        /* -- tu używamy tego samego klucza co w ResourceManager -- */
        resLine("CT-scanner",  "CT");
        resLine("RTG",         "XRAY");
        resLine("USG",         "USG");
        resLine("Sale oper.",  "OR");
        resLine("Anestezjol.", "ANEST");
        resLine("Lozka OIOM",  "ICU");
        resLine("Defibrylatory","DEFIB");
        resLine("Echo serca",  "ECHO");
        resLine("Respirator",  "VENT");
        resLine("Dializator",  "DIAL");
        resLine("Endoskop",    "ENDO");
        resLine("Laboratorium","LAB");
        resLine("Zest. krwi",  "BLOOD");
        resLine("Neuro konsult.","NEURO");
        resLine("Zest. trauma","TRAUMA_KIT");
        resLine("Ortho-set",   "ORTHO_SET");

        std::cout << "====================================================================\n"
                  << CLR_RESET;
    }

    // PODSUMOWANIE
    std::cout << CLR_YELLOW
              << "\n=========== SYMULACJA ZAKONCZONA ===========\n"
              << "Czas trwania symulacji: "
              << hhmmss(std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::steady_clock::now() - start_)) << "\n\n";

    std::cout << ">>> Statystyki pacjentow:\n"
              << "    Obsluzonych w triage: " << triage_.done() << "\n"
              << "    Chirurgia:   " << surg_.getServed() << "\n"
              << "    Ortopedia:   " << ortho_.getServed() << "\n"
              << "    Kardiologia: " << cardio_.getServed() << "\n\n";

    // Statystyki z loggera (czasy i sprzęt)
    gPatientLogger.summary(std::cout);

    std::cout << "Pelna lista pacjentow zapisana w: out/patients.csv\n";
    std::cout << "===============================================\n" << CLR_RESET;
}

std::string DashboardMonitor::hhmmss(std::chrono::seconds s) {
    int h = int(s.count()/3600), m = int((s.count()%3600)/60), sec = int(s.count()%60);
    char buf[16];
    std::snprintf(buf,sizeof(buf),"%02d:%02d:%02d",h,m,sec);
    return buf;
}
