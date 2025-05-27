#ifndef SOR_CONFIG_H
#define SOR_CONFIG_H

#include "configuration_watcher.h"
#include <string>

struct Config
{
    /* ---------- arrival ---------- */
    double lambdaPerSec      = 1.0;
    bool   massEnabled       = false;
    int    massAtSec         = 2;
    double massRatePerSec    = 100.0;

    /* ---------- triage_distribution ---------- */
    double pRed    = .10;
    double pYellow = .20;
    double pGreen  = .50;
    double pBlue   = .20;

    /* ---------- staff ---------- */
    int chirurg      = 8;
    int ortopeda     = 6;
    int kardiolog    = 7;
    int consultSurg  = 4;
    int consultCard  = 3;

    /* ---------- equipment ---------- */
    int ct           = 2;
    int xray         = 3;
    int usg          = 1;
    int oroom        = 2;
    int anest        = 4;
    int icuBeds      = 3;
    int defib        = 2;
    int echo         = 1;
    int vent         = 2;
    int dial         = 1;
    int endo         = 1;
    int lab          = 3;
    int blood        = 4;
    int neuro        = 1;
    int traumaKit    = 3;
    int orthoSet     = 2;
    int ekg          = 2;

    /* ---------- timeouts ---------- */
    int deadlockMs   = 8000;
    int backoffMinMs = 1000;
    int backoffMaxMs = 3000;

    static Config load(const ConfigurationWatcher& w);
};

#endif /* SOR_CONFIG_H */
