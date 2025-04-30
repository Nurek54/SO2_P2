#include "resource_manager.h"

/* ─────────── globalny wskaźnik ─────────── */
ResourceManager* gResMgr = nullptr;

/* ------------- Semaphore ------------- */
void Semaphore::acquire(int n)
{
    std::unique_lock<std::mutex> lk(mtx_);
    cv_.wait(lk, [&]{ return count_ >= n; });
    count_ -= n;
}
void Semaphore::release(int n)
{
    {
        std::lock_guard<std::mutex> lk(mtx_);
        count_ += n;
    }
    cv_.notify_all();
}
int Semaphore::available() const
{
    std::lock_guard<std::mutex> lk(mtx_);
    return count_;
}

/* ------------- ResourceManager ------------- */
ResourceManager::ResourceManager(const Config& c)
{
    auto add = [&](const std::string& k, int n)
    {
        sem_.try_emplace(k, n);   // tu powstaje Semaphore
        total_.emplace(k, n);
    };

    add("CT"   , c.ct);
    add("XRAY" , c.xray);
    add("USG"  , c.usg);
    add("OR"   , c.oroom);
    add("ANEST", c.anest);
    add("ICU"  , c.icuBeds);

    gResMgr = this;
}

void ResourceManager::acquire(const std::string& k, int n) { sem_.at(k).acquire(n); }
void ResourceManager::release(const std::string& k, int n) { sem_.at(k).release(n); }

double ResourceManager::getUtilization(const std::string& k) const
{
    int tot = total(k);
    return tot ? static_cast<double>(inUse(k)) / tot : 0.0;
}
