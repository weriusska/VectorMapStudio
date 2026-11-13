#pragma once
#include "Obiekt.h"

class Trawa : public ObiektMapy {
public:
    Trawa() : ObiektMapy(Kategoria::Teren) {}
    std::string pobierzNazwe() const override { return "Trawa"; }
    bool czyMoznaWejsc() const override { return true; }
    char pobierzSymbol() const override { return '.'; }
};

class Sciana : public ObiektMapy {
public:
    Sciana() : ObiektMapy(Kategoria::Infrastruktura) {}
    std::string pobierzNazwe() const override { return "Sciana"; }
    bool czyMoznaWejsc() const override { return false; }
    char pobierzSymbol() const override { return '#'; }
};

class Woda : public ObiektMapy {
public:
    Woda() : ObiektMapy(Kategoria::Teren) {}
    std::string pobierzNazwe() const override { return "Woda"; }
    bool czyMoznaWejsc() const override { return false; }
    char pobierzSymbol() const override { return '~'; }
};

class Ziemia : public ObiektMapy {
public:
    Ziemia() : ObiektMapy(Kategoria::Teren) {}
    std::string pobierzNazwe() const override { return "Ziemia"; }
    bool czyMoznaWejsc() const override { return true; }
    char pobierzSymbol() const override { return ','; }
};

class KamiennaPodloga : public ObiektMapy {
public:
    KamiennaPodloga() : ObiektMapy(Kategoria::Teren) {}
    std::string pobierzNazwe() const override { return "KamiennaPodloga"; }
    bool czyMoznaWejsc() const override { return true; }
    char pobierzSymbol() const override { return '='; }
};

class Drzwi : public ObiektMapy {
private:
    bool otwarte = false;
public:
    Drzwi() : ObiektMapy(Kategoria::Infrastruktura) {}

    std::string pobierzNazwe() const override {
        return otwarte ? "DrzwiOtwarte" : "Drzwi";
    }

    bool czyMoznaWejsc() const override { return otwarte; }
    char pobierzSymbol() const override { return otwarte ? 'd' : 'D'; }

    void zmienStan() noexcept { otwarte = !otwarte; }
};