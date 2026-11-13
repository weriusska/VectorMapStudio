#pragma once
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <regex>
#include <filesystem>
#include <stdexcept>
#include "Kafelek.h"
#include "Zarzadca.h"

namespace fs = std::filesystem;

class SystemZapisu {
public:
    static bool zapiszDoPliku(
        const std::string& nazwaMapy,
        int szerokosc,
        int wysokosc,
        const std::vector<std::unique_ptr<ObiektMapy>>& siatka)
    {
        fs::path folderGlowny = "ZapisaneMapy";

        if (!fs::exists(folderGlowny)) {
            fs::create_directory(folderGlowny);
        }

        fs::path pelnaSciezka = folderGlowny / (nazwaMapy + ".txt");
        std::ofstream plik(pelnaSciezka);

        if (!plik.is_open()) {
            return false;
        }

        plik << "Nazwa_Mapy: " << nazwaMapy << "\n";
        plik << "Wymiary: " << szerokosc << " " << wysokosc << "\n";

        for (int y = 0; y < wysokosc; ++y) {
            for (int x = 0; x < szerokosc; ++x) {
                size_t idx = static_cast<size_t>(y) * static_cast<size_t>(szerokosc) + static_cast<size_t>(x);
                if (idx < siatka.size() && siatka[idx] != nullptr) {
                    plik << siatka[idx]->pobierzSymbol();
                }
                else {
                    plik << '.';
                }
            }
            plik << "\n";
        }

        return true;
    }

    static bool wczytajZPliku(const std::string& sciezkaDoPliku, ZarzadcaMapy& silnik) {
        std::ifstream plik(sciezkaDoPliku);
        if (!plik.is_open()) return false;

        std::string linia;

        if (!std::getline(plik, linia)) return false;
        if (!std::getline(plik, linia)) return false;

        std::regex wzorWymiarow(R"(Wymiary:\s*(\d+)\s+(\d+))");
        std::smatch dopasowanie;

        int szer = 0, wys = 0;
        if (std::regex_search(linia, dopasowanie, wzorWymiarow)) {
            try {
                szer = std::stoi(dopasowanie[1].str());
                wys = std::stoi(dopasowanie[2].str());
            }
            catch (const std::exception&) {
                return false;
            }
        }
        else {
            return false;
        }

        if (wys < 10 || wys > 200 || szer < 10 || szer > 200) {
            return false;
        }

        silnik.zmienRozmiar(szer, wys);

        std::string regula = "^[.#~,=Dd]{" + std::to_string(szer) + "}$";
        std::regex wzorMapy(regula);

        for (int y = 0; y < wys; ++y) {
            if (!std::getline(plik, linia)) return false;

            if (!std::regex_match(linia, wzorMapy)) {
                return false;
            }

            for (int x = 0; x < szer; ++x) {
                char znak = linia[x];
                switch (znak) {
                case '.': silnik.postawObiekt(x, y, std::make_unique<Trawa>()); break;
                case '#': silnik.postawObiekt(x, y, std::make_unique<Sciana>()); break;
                case '~': silnik.postawObiekt(x, y, std::make_unique<Woda>()); break;
                case ',': silnik.postawObiekt(x, y, std::make_unique<Ziemia>()); break;
                case '=': silnik.postawObiekt(x, y, std::make_unique<KamiennaPodloga>()); break;
                case 'D':
                case 'd': {
                    auto drzwi = std::make_unique<Drzwi>();
                    if (znak == 'd') {
                        drzwi->zmienStan();
                    }
                    silnik.postawObiekt(x, y, std::move(drzwi));
                    break;
                }
                default: break;
                }
            }
        }

        return true;
    }
};