# 🏥 SOR - Symulator Szpitalnego Oddziału Ratunkowego

> **PL** Wielowątkowa symulacja SOR w C++20 | **EN** Multithreaded Emergency Room simulation in C++20

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.30+-064F8C?logo=cmake)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey)

---

## Spis treści / Table of Contents

- [🇵🇱 Wersja polska](#-wersja-polska)
- [🇬🇧 English version](#-english-version)

---

# 🇵🇱 Wersja polska

## O projekcie

SOR to symulacja pracy Szpitalnego Oddziału Ratunkowego, w której każdy lekarz, każda pielęgniarka i każdy zasób medyczny działa we własnym wątku. Pacjenci napływają losowo (rozkład Poissona), przechodzą segregację medyczną (triage), trafiają na właściwy oddział i są leczeni z użyciem współdzielonego sprzętu - dokładnie tak, jak w prawdziwym SOR, tylko szybciej.

Projekt powstał jako demonstracja mechanizmów współbieżności: semaforów, muteksów, zmiennych warunkowych, kolejek priorytetowych i detekcji zakleszczeń.

## Jak to działa

```
Pacjent ──▶ Triage (4 pielęgniarki) ──▶ Oddział ──▶ Lekarz ──▶ Wypis
                │                            │            │
                │  priorytet + choroba        │            ▼
                │  RED/YELLOW/GREEN/BLUE      │     ResourceManager
                │                             │     (CT, RTG, sale op.,
                ▼                             │      łóżka OIOM, EKG…)
         ArrivalDispatcher                    │
         (Poisson + zdarzenia masowe)         ▼
                                        SafeQueue z aging
                                        (awans priorytetu)
```

**Przebieg:**

1. `ArrivalDispatcher` generuje pacjentów wg rozkładu Poissona. W zadanej sekundzie może wywołać zdarzenie masowe (np. wypadek - nagły napływ wielu osób).
2. `Triage` - 4 wątki pielęgniarek losują choroby (21 możliwych) i przydzielają priorytet (RED → BLUE) wg konfigurowalnego rozkładu.
3. Pacjent trafia do jednego z trzech oddziałów: **Chirurgia**, **Ortopedia** lub **Kardiologia**.
4. `Doctor` (wątek lekarza) pobiera pacjenta z kolejki priorytetowej, zajmuje potrzebny sprzęt przez `ResourceManager` (semafory), leczy, zwalnia zasoby.
5. `SafeQueue` implementuje **aging** - pacjenci czekający zbyt długo awansują w priorytecie.
6. `DeadlockDetector` co sekundę sprawdza, czy zasoby nie utknęły bez postępu.
7. `DashboardMonitor` rysuje w terminalu tabelę oddziałów i zasobów, odświeżaną co 1 s.

## Mechanizmy współbieżności

| Mechanizm | Gdzie | Po co |
|---|---|---|
| Semafory (`Semaphore`) | `ResourceManager` | Kontrola dostępu do 17 typów sprzętu medycznego |
| Mutex + condition_variable | `SafeQueue`, `Triage`, `PatientLogger` | Synchronizacja kolejek |
| `std::atomic` | `Hospital`, `Department`, `Triage` | Flagi `running`, liczniki `busy`/`served` |
| Zasoby odnawialne | `ResourceManager` | Po użyciu wracają po cool-downie (LAB: 1.5 s, BLOOD: 1.8 s) |
| Aging | `SafeQueue` + `Department` | BLUE→GREEN (18 s), GREEN→YELLOW (12 s), YELLOW→RED (8 s) |
| Detekcja zakleszczeń | `DeadlockDetector` | 3 cykle bez zmiany stanu = potencjalny deadlock |

## Moduły

| Moduł | Pliki | Rola |
|---|---|---|
| **ArrivalDispatcher** | `arrival_dispatcher.cpp` | Generowanie pacjentów + zdarzenia masowe |
| **Triage** | `triage.cpp/h` | 4 pielęgniarki - segregacja, losowanie chorób |
| **Department** | `department.cpp/h` | Oddział z pulą lekarzy i kolejką priorytetową |
| **Doctor** | `doctor.cpp/h` | Wątek lekarza - diagnostyka, leczenie, zwalnianie sprzętu |
| **SafeQueue** | `safe_queue.cpp/h` | Kolejka 4-poziomowa, thread-safe, z aging |
| **ResourceManager** | `resource_manager.cpp/h` | 17 zasobów, semafory, cool-down odnawialnych |
| **DeadlockDetector** | `deadlock_detector.cpp/h` | Cykliczna detekcja zakleszczeń |
| **DashboardMonitor** | `monitor.cpp/h` | Dashboard terminalowy (co 1 s) |
| **MetricsCollector** | `metrics_collector.cpp/h` | Zapis metryk do CSV co 10 s |
| **PatientLogger** | `patient_logger.cpp/h` | Asynchroniczny log pacjentów do CSV |
| **Hospital** | `hospital.cpp/h` | Orkiestracja - tworzy i łączy wszystkie moduły |
| **ConfigurationWatcher** | `configuration_watcher.cpp/h` | Parser `config.txt` |

## Konfiguracja

Wszystkie parametry w pliku `config.txt`:

```ini
# Tempo napływu
arrival.lambda_per_sec             = 0.8
arrival.mass_casualty.enabled      = true
arrival.mass_casualty.at_second    = 300
arrival.mass_casualty.rate_per_sec = 6

# Rozkład priorytetów
triage_distribution.red    = 0.10
triage_distribution.yellow = 0.20
triage_distribution.green  = 0.50
triage_distribution.blue   = 0.20

# Personel
staff.chirurg   = 3
staff.ortopeda  = 4
staff.kardiolog = 3

# Sprzęt (przykłady)
equipment.ct       = 1
equipment.or       = 2
equipment.icu_beds = 5
equipment.ekg      = 2
```

## Wyjścia

- **Dashboard w terminalu** - stan oddziałów, kolejek, zasobów w czasie rzeczywistym
- **`out/patients.csv`** - historia każdego pacjenta (choroby, priorytet, sprzęt, czas oczekiwania i leczenia)
- **`out/metrics.csv`** - szereg czasowy (kolejki oddziałów, wykorzystanie CT i sal operacyjnych)
- **Podsumowanie końcowe** - średnie czasy, statystyki sprzętu, przestoje oddziałów

## Kompilacja i uruchomienie

```bash
git clone https://github.com/<user>/sor-simulation.git
cd sor-simulation
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
./SO2_P2                   # czyta config.txt z katalogu roboczego
./SO2_P2 sciezka/do/cfg    # lub własny plik konfiguracyjny
```

Zatrzymanie: **Ctrl+C** - program zakończy się czysto i wypisze podsumowanie.

## Wymagania

- Kompilator C++20 (GCC 12+, Clang 15+, MSVC 2022+)
- CMake 3.30+
- Wątki POSIX (Linux) lub Win32 (Windows)

---

# 🇬🇧 English version

## About

SOR is a simulation of a Hospital Emergency Room where every doctor, nurse, and medical resource runs in its own thread. Patients arrive randomly (Poisson distribution), go through triage, are assigned to the correct department, and get treated using shared equipment - just like a real ER, only faster.

The project demonstrates concurrency mechanisms in practice: semaphores, mutexes, condition variables, priority queues, and deadlock detection.

## How it works

```
Patient ──▶ Triage (4 nurses) ──▶ Department ──▶ Doctor ──▶ Discharge
                │                      │             │
                │  priority + disease   │             ▼
                │  RED/YELLOW/GREEN/BLUE│      ResourceManager
                │                      │      (CT, X-ray, OR,
                ▼                      │       ICU beds, ECG…)
         ArrivalDispatcher             │
         (Poisson + mass casualty)     ▼
                                  SafeQueue with aging
                                  (priority promotion)
```

**Flow:**

1. `ArrivalDispatcher` generates patients following a Poisson distribution. At a configured time, it can trigger a mass casualty event (sudden surge of patients).
2. `Triage` - 4 nurse threads randomly assign diseases (21 possible) and priorities (RED → BLUE) according to a configurable distribution.
3. The patient is routed to one of three departments: **Surgery**, **Orthopedics**, or **Cardiology**.
4. `Doctor` (a thread per doctor) pops a patient from the priority queue, acquires needed equipment via `ResourceManager` (semaphores), treats the patient, releases resources.
5. `SafeQueue` implements **aging** - patients waiting too long get promoted in priority.
6. `DeadlockDetector` periodically checks whether resources are stuck with no progress.
7. `DashboardMonitor` draws a live terminal table of departments and resources, refreshed every second.

## Concurrency mechanisms

| Mechanism | Where | Purpose |
|---|---|---|
| Semaphores (`Semaphore`) | `ResourceManager` | Access control for 17 types of medical equipment |
| Mutex + condition_variable | `SafeQueue`, `Triage`, `PatientLogger` | Queue synchronization |
| `std::atomic` | `Hospital`, `Department`, `Triage` | `running` flags, `busy`/`served` counters |
| Renewable resources | `ResourceManager` | Return to the pool after a cooldown (LAB: 1.5 s, BLOOD: 1.8 s) |
| Aging | `SafeQueue` + `Department` | BLUE→GREEN (18 s), GREEN→YELLOW (12 s), YELLOW→RED (8 s) |
| Deadlock detection | `DeadlockDetector` | 3 unchanged cycles = potential deadlock |

## Modules

| Module | Files | Role |
|---|---|---|
| **ArrivalDispatcher** | `arrival_dispatcher.cpp` | Patient generation + mass casualty events |
| **Triage** | `triage.cpp/h` | 4 nurses - triage, random disease assignment |
| **Department** | `department.cpp/h` | Department with a doctor pool and priority queue |
| **Doctor** | `doctor.cpp/h` | Doctor thread - diagnosis, treatment, resource release |
| **SafeQueue** | `safe_queue.cpp/h` | 4-level thread-safe queue with aging |
| **ResourceManager** | `resource_manager.cpp/h` | 17 resources, semaphores, renewable cooldowns |
| **DeadlockDetector** | `deadlock_detector.cpp/h` | Periodic deadlock detection |
| **DashboardMonitor** | `monitor.cpp/h` | Terminal dashboard (every 1 s) |
| **MetricsCollector** | `metrics_collector.cpp/h` | Metrics to CSV every 10 s |
| **PatientLogger** | `patient_logger.cpp/h` | Async patient logging to CSV |
| **Hospital** | `hospital.cpp/h` | Orchestration - creates and joins all modules |
| **ConfigurationWatcher** | `configuration_watcher.cpp/h` | `config.txt` parser |

## Configuration

All parameters live in `config.txt`:

```ini
# Arrival rate
arrival.lambda_per_sec             = 0.8
arrival.mass_casualty.enabled      = true
arrival.mass_casualty.at_second    = 300
arrival.mass_casualty.rate_per_sec = 6

# Priority distribution
triage_distribution.red    = 0.10
triage_distribution.yellow = 0.20
triage_distribution.green  = 0.50
triage_distribution.blue   = 0.20

# Staff
staff.chirurg   = 3
staff.ortopeda  = 4
staff.kardiolog = 3

# Equipment (examples)
equipment.ct       = 1
equipment.or       = 2
equipment.icu_beds = 5
equipment.ekg      = 2
```

## Outputs

- **Terminal dashboard** - real-time view of departments, queues, and resource utilization
- **`out/patients.csv`** - per-patient record (diseases, priority, equipment, wait and treatment times)
- **`out/metrics.csv`** - time series (department queues, CT and OR utilization)
- **Final summary** - average times, equipment stats, department idle periods

## Build & Run

```bash
git clone https://github.com/<user>/sor-simulation.git
cd sor-simulation
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
./SO2_P2                   # reads config.txt from working directory
./SO2_P2 path/to/config    # or a custom config file
```

Stop with **Ctrl+C** - the program shuts down gracefully and prints a summary.

## Requirements

- C++20 compiler (GCC 12+, Clang 15+, MSVC 2022+)
- CMake 3.30+
- POSIX threads (Linux) or Win32 threads (Windows)

---

<p align="center"><i>Projekt akademicki - Systemy Operacyjne 2 · Academic project - Operating Systems 2</i></p>
