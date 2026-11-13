#pragma once
#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <ranges>
#include <functional>
#include "Kafelek.h"

enum class TypPedzla { Trawa, Sciana, Woda, Ziemia, Chodnik, Drzwi };

class ZarzadcaMapy {
private:
    int szerokosc = 0;
    int wysokosc = 0;
    std::vector<std::unique_ptr<ObiektMapy>> siatka;
    std::string aktualnaNazwaMapy = "BrakNazwy";
    TypPedzla aktualnyPedzel = TypPedzla::Sciana;

    std::atomic<bool> dzialaAutozapis{ false };
    std::thread watekAutozapisu;
    mutable std::mutex mutexSiatki;

    std::function<void()> akcjaPoAutozapisie = nullptr;
    // Wzorzec Observer (MVC): powiadamia widok o zmianie kafelka (x, y, nazwaObiektu)
    std::function<void(int, int, const std::string&)> akcjaPoZmianieKafelka = nullptr;

    void petlaAutozapisu();

    constexpr bool czyWGranicach(int x, int y) const noexcept {
        return x >= 0 && x < szerokosc && y >= 0 && y < wysokosc;
    }

    constexpr size_t indeks(int x, int y) const noexcept {
        return static_cast<size_t>(y) * static_cast<size_t>(szerokosc) + static_cast<size_t>(x);
    }

public:
    ZarzadcaMapy() = default;

    ZarzadcaMapy(int s, int w) : szerokosc(s), wysokosc(w) {
        dzialaAutozapis = true;
        siatka.resize(static_cast<size_t>(szerokosc) * wysokosc);
        for (size_t i = 0; i < siatka.size(); ++i) {
            siatka[i] = std::make_unique<Trawa>();
        }
        watekAutozapisu = std::thread(&ZarzadcaMapy::petlaAutozapisu, this);
    }

    ~ZarzadcaMapy() {
        dzialaAutozapis = false;
        if (watekAutozapisu.joinable()) {
            watekAutozapisu.join();
        }
    }

    ZarzadcaMapy(const ZarzadcaMapy&) = delete;
    ZarzadcaMapy& operator=(const ZarzadcaMapy&) = delete;

    ZarzadcaMapy(ZarzadcaMapy&&) noexcept = default;
    ZarzadcaMapy& operator=(ZarzadcaMapy&&) noexcept = default;

    void ustawAkcjePoAutozapisie(std::function<void()> akcja) {
        akcjaPoAutozapisie = std::move(akcja);
    }

    void ustawObserwatoraMapy(std::function<void(int, int, const std::string&)> obserwator) {
        akcjaPoZmianieKafelka = std::move(obserwator);
    }

    void ustawNazweMapy(const std::string& nowaNazwa) { aktualnaNazwaMapy = nowaNazwa; }
    std::string pobierzNazweMapy() const { return aktualnaNazwaMapy; }

    int pobierzSzerokosc() const noexcept { return szerokosc; }
    int pobierzWysokosc() const noexcept { return wysokosc; }

    void postawObiekt(int x, int y, std::unique_ptr<ObiektMapy> nowyObiekt) {
        std::string nazwaNowegoObiektu;
        {
            std::lock_guard<std::mutex> lock(mutexSiatki);
            if (!czyWGranicach(x, y) || !nowyObiekt) return;
            nazwaNowegoObiektu = nowyObiekt->pobierzNazwe();
            siatka[indeks(x, y)] = std::move(nowyObiekt);
        }
        // Wywo³anie obserwatora poza sekcj¹ krytyczn¹ muteksu
        if (akcjaPoZmianieKafelka) {
            akcjaPoZmianieKafelka(x, y, nazwaNowegoObiektu);
        }
    }

    void namalujAktualnymPedzlem(int x, int y) {
        switch (aktualnyPedzel) {
        case TypPedzla::Trawa:   postawObiekt(x, y, std::make_unique<Trawa>()); break;
        case TypPedzla::Sciana:  postawObiekt(x, y, std::make_unique<Sciana>()); break;
        case TypPedzla::Woda:    postawObiekt(x, y, std::make_unique<Woda>()); break;
        case TypPedzla::Ziemia:  postawObiekt(x, y, std::make_unique<Ziemia>()); break;
        case TypPedzla::Chodnik: postawObiekt(x, y, std::make_unique<KamiennaPodloga>()); break;
        case TypPedzla::Drzwi:   postawObiekt(x, y, std::make_unique<Drzwi>()); break;
        }
    }

    char pobierzSymbolZMapy(int x, int y) const {
        std::lock_guard<std::mutex> lock(mutexSiatki);
        if (czyWGranicach(x, y)) {
            const auto& obiekt = siatka[indeks(x, y)];
            if (obiekt != nullptr) {
                return obiekt->pobierzSymbol();
            }
        }
        return ' ';
    }

    std::string pobierzNazweZMapy(int x, int y) const {
        std::lock_guard<std::mutex> lock(mutexSiatki);
        if (czyWGranicach(x, y)) {
            const auto& obiekt = siatka[indeks(x, y)];
            if (obiekt != nullptr) {
                return obiekt->pobierzNazwe();
            }
        }
        return "Trawa";
    }

    void zmienRozmiar(int nowaSzerokosc, int nowaWysokosc) {
        std::lock_guard<std::mutex> lock(mutexSiatki);
        szerokosc = nowaSzerokosc;
        wysokosc = nowaWysokosc;
        siatka.clear();
        siatka.resize(static_cast<size_t>(szerokosc) * wysokosc);
    }

    int policzObiektyKategorii(Kategoria szukanaKat) const {
        std::lock_guard<std::mutex> lock(mutexSiatki);
        auto przefiltrowanyWidok = siatka
            | std::views::filter([szukanaKat](const std::unique_ptr<ObiektMapy>& obiekt) {
            return obiekt != nullptr && obiekt->pobierzKategorie() == szukanaKat;
                });
        return static_cast<int>(std::ranges::distance(przefiltrowanyWidok));
    }

    void przejmijDane(ZarzadcaMapy& inne) {
        std::scoped_lock lock(mutexSiatki, inne.mutexSiatki);
        this->szerokosc = inne.szerokosc;
        this->wysokosc = inne.wysokosc;
        this->siatka = std::move(inne.siatka);
        this->aktualnaNazwaMapy = std::move(inne.aktualnaNazwaMapy);
        this->aktualnyPedzel = inne.aktualnyPedzel;
    }

    void ustawPedzel(TypPedzla typ) noexcept { aktualnyPedzel = typ; }
    TypPedzla pobierzPedzel() const noexcept { return aktualnyPedzel; }

    const std::vector<std::unique_ptr<ObiektMapy>>& pobierzSiatke() const noexcept {
        return siatka;
    }

    bool przelaczStanDrzwi(int x, int y) {
        std::string nowaNazwa;
        bool przelaczono = false;
        {
            std::lock_guard<std::mutex> lock(mutexSiatki);
            if (czyWGranicach(x, y)) {
                if (Drzwi* drzwi = dynamic_cast<Drzwi*>(siatka[indeks(x, y)].get())) {
                    drzwi->zmienStan();
                    nowaNazwa = drzwi->pobierzNazwe();
                    przelaczono = true;
                }
            }
        }
        if (przelaczono && akcjaPoZmianieKafelka) {
            akcjaPoZmianieKafelka(x, y, nowaNazwa);
        }
        return przelaczono;
    }
};