#include "doctor.h"
#include "resource_manager.h"
#include "patient_logger.h"
#include <random>
#include <chrono>
#include <iostream>
#include <vector>
#include <array>

extern ResourceManager* gResMgr;

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

void Doctor::join() {
    if (thread_.joinable()) thread_.join();
}

void Doctor::work() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> contactTime(2, 5);
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    while (running_) {
        Patient p;
        if (!queue_.pop(p, running_)) break;

        auto start = std::chrono::steady_clock::now();
        long waitMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                start - p.queued).count();
        std::vector<std::string> equip;

        auto use = [&](const char* key){
            equip.push_back(key);
            gResMgr->acquire(key);
        };
        auto free = [&](const char* key){
            gResMgr->release(key);
        };

        ++busy_;
        std::this_thread::sleep_for(std::chrono::seconds(contactTime(gen)));

        if (dept_ == DepartmentID::SURGERY) {
            if (prob(gen) < 0.25) {
                use("ANEST");
                use("OR");
                std::uniform_int_distribution<int> opTime(10, 20);
                std::this_thread::sleep_for(std::chrono::seconds(opTime(gen)));
                free("OR");
                free("ANEST");
                if (prob(gen) < 0.5) {
                    use("ICU");
                    std::this_thread::sleep_for(std::chrono::seconds(30));
                    free("ICU");
                }
            } else {
                use("CT");
                std::uniform_int_distribution<int> scanTime(8, 15);
                std::this_thread::sleep_for(std::chrono::seconds(scanTime(gen)));
                free("CT");
            }
        }
        else if (dept_ == DepartmentID::ORTHOPEDIC) {
            if (prob(gen) < 0.30) {
                use("OR");
                std::uniform_int_distribution<int> opTime(8, 12);
                std::this_thread::sleep_for(std::chrono::seconds(opTime(gen)));
                free("OR");
            } else {
                use("XRAY");
                std::uniform_int_distribution<int> xTime(3, 6);
                std::this_thread::sleep_for(std::chrono::seconds(xTime(gen)));
                free("XRAY");
                use("TRAUMA_KIT");
                use("ORTHO_SET");
                std::this_thread::sleep_for(std::chrono::seconds(5));
                free("ORTHO_SET");
                free("TRAUMA_KIT");
            }
        }
        else { // CARDIO
            use("EKG");
            std::this_thread::sleep_for(std::chrono::seconds(2));
            free("EKG");
            if (prob(gen) < 0.50) {
                use("ECHO");
                std::this_thread::sleep_for(std::chrono::seconds(4));
                free("ECHO");
            }
            if (prob(gen) < 0.30) {
                use("DEFIB");
                std::this_thread::sleep_for(std::chrono::seconds(1));
                free("DEFIB");
            }
        }

        struct ToolSpec {
            const char* key;
            double      p;
            int         minMs;
            int         maxMs;
        };
        static const std::array<ToolSpec,7> tools = {{
                                                             { "VENT",  0.25, 4000,  8000 },
                                                             { "DIAL",  0.20, 6000, 12000 },
                                                             { "LAB",   0.35, 2000,  4000 },
                                                             { "BLOOD", 0.30, 1500,  3000 },
                                                             { "ENDO",  0.15, 5000,  8000 },
                                                             { "NEURO", 0.12, 2500,  3500 },
                                                             { "USG",   0.18, 3000,  5000 }
                                                     }};

        for (const auto& t : tools) {
            if (prob(gen) < t.p) {
                use(t.key);
                std::uniform_int_distribution<int> dur(t.minMs, t.maxMs);
                std::this_thread::sleep_for(std::chrono::milliseconds(dur(gen)));
                free(t.key);
            }
        }

        auto end = std::chrono::steady_clock::now();
        long treatMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                end - start).count();
        gPatientLogger.log(p, equip, waitMs, treatMs);

        ++served_;
        --busy_;
    }
}
