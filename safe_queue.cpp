#include "safe_queue.h"

void SafeQueue::push(const Patient& p) {
    std::lock_guard<std::mutex> lk(mtx);
    pq.push(p);
    cv.notify_one();
}

bool SafeQueue::pop(Patient& out, std::atomic<bool>& running) {
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [&]{ return !pq.empty() || !running; });
    if (!running && pq.empty()) return false;
    out = pq.top();
    pq.pop();
    return true;
}

size_t SafeQueue::size() const {
    std::lock_guard<std::mutex> lk(mtx);
    return pq.size();
}

std::array<int,4> SafeQueue::priorityCounts() const {
    std::lock_guard<std::mutex> lk(mtx);
    auto copy = pq;
    std::array<int,4> counts = {0,0,0,0};
    while (!copy.empty()) {
        counts[static_cast<int>(copy.top().priority)]++;
        copy.pop();
    }
    return counts;
}
