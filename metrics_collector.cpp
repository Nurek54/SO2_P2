#include "metrics_collector.h"
#include <chrono>

MetricsCollector::MetricsCollector(std::atomic<bool>& run,
                                   const Config& cfg,
                                   Department& s, Department& o,
                                   Department& c,
                                   ResourceManager& rm)
        : running_(run), cfg_(cfg),
          surg_(s), ortho_(o), cardio_(c), rm_(rm)
{
    out_.open("out/metrics.csv");
    out_ << "ts;queue_s;queue_o;queue_c;util_ct;util_or\n";
    th_ = std::thread(&MetricsCollector::loop, this);
}

void MetricsCollector::join()
{
    if (th_.joinable()) th_.join();
}

void MetricsCollector::loop()
{
    using namespace std::chrono;
    while (running_)
    {
        std::this_thread::sleep_for( seconds(10) );

        auto ts = duration_cast<seconds>(
                steady_clock::now().time_since_epoch()).count();

        out_ << ts << ';'
             << surg_.queueLen()  << ';'
             << ortho_.queueLen() << ';'
             << cardio_.queueLen()<< ';'
             << rm_.available("CT") << ';'
             << rm_.available("OR") << '\n';

        out_.flush();
    }
}
