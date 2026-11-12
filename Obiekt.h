#pragma once
#include <string>


enum class Kategoria { Teren, Infrastruktura };


class ObiektMapy {
protected:
    Kategoria kategoriaObiektu;
public:
    ObiektMapy(Kategoria kat) : kategoriaObiektu(kat) {}
    virtual ~ObiektMapy() = default;

    ObiektMapy(const ObiektMapy&) = delete;
    ObiektMapy& operator=(const ObiektMapy&) = delete;

    virtual std::string pobierzNazwe() const = 0;
    virtual bool czyMoznaWejsc() const = 0;
    virtual char pobierzSymbol() const = 0;

    Kategoria pobierzKategorie() const { return kategoriaObiektu; }
};