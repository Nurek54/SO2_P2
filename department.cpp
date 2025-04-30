#include "department.h"

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
