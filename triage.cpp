#include "triage.h"
#include "patient.h"
#include <random>
#include <chrono>
#include <array>

/* ───────────── słownik chorób ───────────── */
struct Disease { std::string name; DepartmentID dept; };

static const std::array<Disease, 21> D = {{
                                                  /* chirurgia */   {"Zapalenie wyrostka",       DepartmentID::SURGERY},
                                                                    {"Uraz jamy brzusznej",      DepartmentID::SURGERY},
                                                                    {"Krwotok wewnętrzny",       DepartmentID::SURGERY},
                                                                    {"Kolka żółciowa",           DepartmentID::SURGERY},
                                                                    {"Niedrożność jelit",        DepartmentID::SURGERY},
                                                                    {"Przepuklina",              DepartmentID::SURGERY},
                                                                    {"Kamica nerkowa",           DepartmentID::SURGERY},
                                                  /* ortopedia */   {"Złamanie nogi",            DepartmentID::ORTHOPEDIC},
                                                                    {"Złamanie ręki",            DepartmentID::ORTHOPEDIC},
                                                                    {"Zwichnięcie barku",        DepartmentID::ORTHOPEDIC},
                                                                    {"Skręcenie kostki",         DepartmentID::ORTHOPEDIC},
                                                                    {"Uszkodzenie więzadeł",     DepartmentID::ORTHOPEDIC},
                                                                    {"Ból kręgosłupa",           DepartmentID::ORTHOPEDIC},
                                                                    {"Złamanie szyjki kości",    DepartmentID::ORTHOPEDIC},
                                                  /* kardiologia */ {"Zawał serca",              DepartmentID::CARDIO},
                                                                    {"Arytmia",                  DepartmentID::CARDIO},
                                                                    {"Ból w klatce",             DepartmentID::CARDIO},
                                                                    {"Niewydolność serca",       DepartmentID::CARDIO},
                                                                    {"Przełom nadciśnieniowy",   DepartmentID::CARDIO},
                                                                    {"Omdlenie",                 DepartmentID::CARDIO},
                                                                    {"Kardiomiopatia",           DepartmentID::CARDIO}
                                          }};

/* ───────────── ctor ───────────── */
Triage::Triage(Department& s, Department& o, Department& c,
               std::atomic<bool>& run, const Config& cfg,
               int nurseCount)
        : surg_(s), ortho_(o), cardio_(c),
          running_(run),
          pRed_(cfg.pRed), pYellow_(cfg.pYellow),
          pGreen_(cfg.pGreen), pBlue_(cfg.pBlue)
{
    for (int i = 0; i < nurseCount; ++i)
        nurses_.emplace_back(&Triage::nurseLoop, this);
}

/* ───────────── interfejs ───────────── */
void Triage::newPatient(int id)
{
    std::lock_guard<std::mutex> lk(mtx_);
    regQueue_.push(id);
    cv_.notify_one();
}

std::size_t Triage::regLen()
{
    std::lock_guard<std::mutex> lk(mtx_);
    return regQueue_.size();
}

void Triage::join()
{
    for (auto& n : nurses_) if (n.joinable()) n.join();
}

/* ───────────── pętla pielęgniarki ───────────── */
void Triage::nurseLoop()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> tTime(100, 300);      // czas segregacji [ms]
    std::uniform_int_distribution<int> idxDist(0, D.size() - 1);
    std::uniform_real_distribution<double> U(0.0, 1.0);

    while (running_)
    {
        /* ---- pobranie pacjenta z rejestracji ---- */
        int id = -1;
        {
            std::unique_lock<std::mutex> lk(mtx_);
            cv_.wait(lk, [&]{ return !regQueue_.empty() || !running_; });
            if (!running_ && regQueue_.empty()) break;
            id = regQueue_.front(); regQueue_.pop();
        }

        ++busy;
        std::this_thread::sleep_for(std::chrono::milliseconds(tTime(gen)));

        /* ---- losowanie chorób (1–2) ---- */
        std::vector<int> picks{ idxDist(gen) };
        if (U(gen) < 0.20) {          // 20 % pacjentów ma dwie choroby
            int j; do { j = idxDist(gen); } while (j == picks[0]);
            picks.push_back(j);
        }

        /* ---- losowanie priorytetu wg rozkładu ---- */
        auto drawPr = [&]{
            double r = U(gen);
            if (r < pRed_)                     return Priority::RED;
            if (r < pRed_ + pYellow_)          return Priority::YELLOW;
            if (r < pRed_ + pYellow_ + pGreen_)return Priority::GREEN;
            return Priority::BLUE;
        };

        Priority top = Priority::BLUE;
        DepartmentID target = D[picks[0]].dept;
        for (int idx : picks) {
            Priority pr = drawPr();
            if (static_cast<int>(pr) < static_cast<int>(top)) {
                top = pr; target = D[idx].dept;
            }
        }

        /* ---- przygotowanie rekordu pacjenta ---- */
        auto now = std::chrono::steady_clock::now();
        Patient p{ id, {}, top, target, now, now };
        for (int idx : picks) p.diseases.push_back(D[idx].name);

        /* ---- przekazanie na odpowiedni oddział ---- */
        switch (target) {
            case DepartmentID::SURGERY:    surg_.addPatient(p);   break;
            case DepartmentID::ORTHOPEDIC: ortho_.addPatient(p);  break;
            case DepartmentID::CARDIO:     cardio_.addPatient(p); break;
        }
        ++processed_;
        --busy;
    }
}
