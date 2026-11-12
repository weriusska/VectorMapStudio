#include "Zarzadca.h"
#include "SystemZapisu.h"

void ZarzadcaMapy::petlaAutozapisu() {
    while (dzialaAutozapis) {
        for (int i = 0; i < 30; ++i) {
            if (!dzialaAutozapis) return;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::string nazwaZapasowa = aktualnaNazwaMapy;
        const std::string sufiks = "_Autosave";

        if (!nazwaZapasowa.ends_with(sufiks)) {
            nazwaZapasowa += sufiks;
        }

        {
            std::lock_guard<std::mutex> lock(mutexSiatki);
            SystemZapisu::zapiszDoPliku(nazwaZapasowa, szerokosc, wysokosc, siatka);
        }

        if (akcjaPoAutozapisie != nullptr) {
            akcjaPoAutozapisie();
        }
    }
}