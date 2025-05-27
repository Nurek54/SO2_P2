#include "department.h"
#include <chrono>

Department::Department(DepartmentID id, int doctorCount,
                       std::atomic<bool>& run)
        : id_(id), totalDoctors_(doctorCount)
{
    for (int i = 1; i <= doctorCount; ++i)
        docs_.push_back(std::make_unique<Doctor>(
                i, id_, queue_, busy, served, run));
}

void Department::join()
{
    for (auto& d : docs_) d->join();
}

void Department::applyAging()
{
    auto now = std::chrono::steady_clock::now();

    static const std::chrono::seconds agingThreshold[] = {
            std::chrono::hours(999),     // RED — nie promujemy
            std::chrono::seconds(45),    // YELLOW → RED
            std::chrono::seconds(90),    // GREEN → YELLOW
            std::chrono::seconds(120)    // BLUE → GREEN
    };

    queue_.applyAging(agingThreshold, now);
}
