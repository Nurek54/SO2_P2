#include "configuration_watcher.h"
#include "config.h"
#include "hospital.h"

int main(int argc, char* argv[])
{
    const std::string cfgPath = (argc > 1) ? argv[1] : "config.txt";

    ConfigurationWatcher watcher(cfgPath);
    Config cfg = Config::load(watcher);

    Hospital h(cfg);
    h.run();
    return 0;
}
