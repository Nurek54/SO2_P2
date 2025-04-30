#ifndef SOR_PATIENT_H
#define SOR_PATIENT_H

#include "types.h"
#include <vector>
#include <chrono>

struct Patient {
    int id;
    std::vector<std::string> diseases;
    Priority priority;
    DepartmentID dept;
    std::chrono::steady_clock::time_point arrived;
    std::chrono::steady_clock::time_point queued;
};

#endif // SOR_PATIENT_H
