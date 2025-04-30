#ifndef SOR_CONFIG_H
#define SOR_CONFIG_H

#include "configuration_watcher.h"
#include <string>

struct Config
{
    /* ---------- arrival ---------- */
    double lambdaPerSec      = 1.0;   // Średnia liczba przyjęć pacjentów na sekundę
    bool   massEnabled       = false; // Czy włączyć zdarzenie masowe (masowy wypadek)
    int    massAtSec         = 0.0;     // Po ilu sekundach od startu następuje zdarzenie masowe
    double massRatePerSec    = 0.0;   // Tempo nowych pacjentów podczas masowego wypadku (pacjentów/sekundę)

    /* ---------- triage_distribution ---------- */
    double pRed    = .10;   // Prawdopodobieństwo przydzielenia pacjentowi priorytetu czerwonego (R)
    double pYellow = .20;   // Prawdopodobieństwo przydzielenia priorytetu żółtego (Y)
    double pGreen  = .50;   // Prawdopodobieństwo przydzielenia priorytetu zielonego (G)
    double pBlue   = .20;   // Prawdopodobieństwo przydzielenia priorytetu niebieskiego (B)

    /* ---------- staff ---------- */
    int chirurg      = 4;   // Liczba lekarzy na oddziale chirurgii
    int ortopeda     = 3;   // Liczba lekarzy na oddziale ortopedii
    int kardiolog    = 4;   // Liczba lekarzy na oddziale kardiologii
    int consultSurg  = 4;   // Liczba konsultantów chirurgicznych (wzywanych z innych oddziałów)
    int consultCard  = 3;   // Liczba konsultantów kardiologicznych

    /* ---------- equipment ---------- */
    int ct           = 2;   // Liczba dostępnych urządzeń CT (tomograf komputerowy)
    int xray         = 3;   // Liczba dostępnych aparatów RTG
    int usg          = 1;   // Liczba dostępnych urządzeń USG
    int oroom        = 2;   // Liczba sal operacyjnych
    int anest        = 4;   // Liczba anestezjologów
    int icuBeds      = 3;   // Liczba dostępnych łóżek na OIOM

    /* ---------- timeouts ---------- */
    int deadlockMs   = 8000; // Maksymalny czas (w ms) zanim uznamy, że zaszło zakleszczenie (deadlock)
    int backoffMinMs = 1000; // Minimalny czas (w ms) losowego opóźnienia przed ponowną próbą po rollbacku
    int backoffMaxMs = 3000; // Maksymalny czas (w ms) losowego opóźnienia przed ponowną próbą po rollbacku

    /*  Ładuje wartości z pliku via ConfigurationWatcher */
    static Config load(const ConfigurationWatcher& w);
};

#endif /* SOR_CONFIG_H */
