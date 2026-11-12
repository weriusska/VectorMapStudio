#pragma once
#include <vector>
#include <memory>
#include <QFileDialog> 
#include <QMessageBox> 
#include "Kafelek.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <ranges>
#include <functional>

enum class TypPedzla { Trawa, Sciana, Woda, Ziemia, Chodnik, Drzwi };

class ZarzadcaMapy {
private:
    int szerokosc = 0;
    int wysokosc = 0;
    std::vector<std::vector<std::unique_ptr<ObiektMapy>>> siatka;
    std::string aktualnaNazwaMapy = "BrakNazwy";
    TypPedzla aktualnyPedzel = TypPedzla::Sciana;

    std::atomic<bool> dzialaAutozapis{ false };
    std::thread watekAutozapisu;
    mutable std::mutex mutexSiatki;

    std::function<void()> akcjaPoAutozapisie = nullptr;
    void petlaAutozapisu();

public:
    ZarzadcaMapy() = default;

    ZarzadcaMapy(int s, int w) : szerokosc(s), wysokosc(w) {
        dzialaAutozapis = true;
        siatka.resize(szerokosc);
        for (int x = 0; x < szerokosc; ++x) {
            siatka[x].resize(wysokosc);
            for (int y = 0; y < wysokosc; ++y) {
                siatka[x][y] = std::make_unique<Trawa>();
            }
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

    void ustawAkcjePoAutozapisie(std::function<void()> akcja) { akcjaPoAutozapisie = akcja; }
    void ustawNazweMapy(const std::string& nowaNazwa) { aktualnaNazwaMapy = nowaNazwa; }
    std::string pobierzNazweMapy() const { return aktualnaNazwaMapy; }

    int pobierzSzerokosc() const { return szerokosc; }
    int pobierzWysokosc() const { return wysokosc; }

    void postawObiekt(int x, int y, std::unique_ptr<ObiektMapy> nowyObiekt) {
        std::lock_guard<std::mutex> lock(mutexSiatki);
        if (x >= 0 && x < szerokosc && y >= 0 && y < wysokosc) {
            siatka[x][y] = std::move(nowyObiekt);
        }
    }

    char pobierzSymbolZMapy(int x, int y) const {
        if (x >= 0 && x < szerokosc && y >= 0 && y < wysokosc) {
            if (siatka[x][y] != nullptr) { 
                return siatka[x][y]->pobierzSymbol();
            }
        }
        return ' ';
    }

    void zmienRozmiar(int nowaSzerokosc, int nowaWysokosc) {
        std::lock_guard<std::mutex> lock(mutexSiatki);
        szerokosc = nowaSzerokosc;
        wysokosc = nowaWysokosc;
        siatka.clear();
        siatka.resize(szerokosc);
        for (int x = 0; x < szerokosc; ++x) {
            siatka[x].resize(wysokosc);
        }
    }

    int policzObiektyKategorii(Kategoria szukanaKat) {
        std::lock_guard<std::mutex> lock(mutexSiatki);
        auto przefiltrowanyWidok = siatka
            | std::views::join
            | std::views::filter([szukanaKat](const std::unique_ptr<ObiektMapy>& obiekt) {
            return obiekt != nullptr && obiekt->pobierzKategorie() == szukanaKat;
                });
        return std::ranges::distance(przefiltrowanyWidok);
    }

    void przejmijDane(ZarzadcaMapy& inne) {
        std::lock_guard<std::mutex> lock(mutexSiatki); 

        this->szerokosc = inne.szerokosc;
        this->wysokosc = inne.wysokosc;
        this->siatka = std::move(inne.siatka); 
        this->aktualnaNazwaMapy = std::move(inne.aktualnaNazwaMapy);
        this->aktualnyPedzel = inne.aktualnyPedzel;
    }

    void ustawPedzel(TypPedzla typ) { aktualnyPedzel = typ; }
    TypPedzla pobierzPedzel() const { return aktualnyPedzel; }

    const std::vector<std::vector<std::unique_ptr<ObiektMapy>>>& pobierzSiatke() const {
        return siatka;
    }

    bool przelaczStanDrzwi(int x, int y) {
        std::lock_guard<std::mutex> lock(mutexSiatki);
        if (x >= 0 && x < szerokosc && y >= 0 && y < wysokosc) {
            if (Drzwi* drzwi = dynamic_cast<Drzwi*>(siatka[x][y].get())) {
                drzwi->zmienStan(); 
                return true;        
            }
        }
        return false; 
    }
};