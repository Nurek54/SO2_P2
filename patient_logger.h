#ifndef SOR_PATIENT_LOGGER_H
#define SOR_PATIENT_LOGGER_H

#include "patient.h"
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <thread>
#include <condition_variable>
#include <queue>
#include <atomic>

struct LogEntry {
    Patient p;
    std::vector<std::string> equip;
    long waitMs;
    long treatMs;
};

class PatientLogger {
public:
    PatientLogger();
    ~PatientLogger();

    void start();                   // uruchom wątek
    void stop();                    // zakończ

    void log(const Patient& p,
             const std::vector<std::string>& equip,
             long waitMs,
             long treatMs);

    void summary(std::ostream& os) const;

private:
    void run();

    std::ofstream out_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<LogEntry> q_;
    std::thread th_;
    std::atomic<bool> running_{false};

    long long totalWait_  = 0;
    long long totalTreat_ = 0;
    long long count_      = 0;
    std::unordered_map<std::string,long long> equipCnt_;
};

extern PatientLogger gPatientLogger;

#endif // SOR_PATIENT_LOGGER_H
