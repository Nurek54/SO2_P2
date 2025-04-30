#ifndef SOR_RESOURCE_MANAGER_H
#define SOR_RESOURCE_MANAGER_H

#include "config.h"
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>

/* ───────────── Semaphore ───────────── */
class Semaphore
{
    int                       count_;
    mutable std::mutex        mtx_;
    std::condition_variable   cv_;

public:
    explicit Semaphore(int n = 0) : count_(n) {}

    void acquire(int n = 1);
    void release(int n = 1);

    int  available() const;
};

/* ───────────── ResourceManager ───────────── */
class ResourceManager
{
    std::map<std::string, Semaphore> sem_;
    std::map<std::string, int>       total_;

public:
    explicit ResourceManager(const Config& cfg);

    void acquire (const std::string& key, int n = 1);
    void release (const std::string& key, int n = 1);

    /* szybkie dostępy */
    int  total     (const std::string& key) const { return total_.at(key); }
    int  available (const std::string& key) const { return sem_.at(key).available(); }
    int  inUse     (const std::string& key) const { return total(key) - available(key); }

    /* aliasy do starego kodu */
    int  getTotal     (const std::string& k) const { return total(k);      }
    int  getAvailable (const std::string& k) const { return available(k);  }
    double getUtilization(const std::string& k) const;

};

/* globalny uchwyt dla lekarzy  */
extern ResourceManager* gResMgr;

#endif /* SOR_RESOURCE_MANAGER_H */
