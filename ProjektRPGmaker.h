#pragma once
#include <QtWidgets/QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QMouseEvent>
#include <QPoint>
#include <QLabel>
#include <QBrush>
#include <unordered_map>
#include <vector>
#include <string>
#include "Zarzadca.h"

class ProjektRPGmaker : public QMainWindow {
    Q_OBJECT

public:
    ProjektRPGmaker(QWidget* parent = nullptr);
    ~ProjektRPGmaker();

    void stworzNowyProjekt(std::string nazwa, int szer, int wys, QString wypelnienie);
    void wczytajProjekt();
    bool wczytajKonkretnyProjekt(const std::string& nazwa);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    static constexpr int ROZMIAR_KRATKI = 32;

    QGraphicsScene* scena;
    QGraphicsView* widok;
    ZarzadcaMapy edytor{ 20, 20 };

    // S³ownik tekstur wczytanych do RAM (NazwaObiektu -> QBrush)
    std::unordered_map<std::string, QBrush> slownikTekstur;

    // P³aski wektor 1D przechowuj¹cy elementy graficzne sceny dla dostêpu O(1)
    std::vector<QGraphicsRectItem*> kafelkiWidoku;

    QPoint pozycjaMyszy;
    QLabel* labelOstatniZapis;

    void inicjalizujSlownikTekstur();
    void przebudujScene();
    void odswiezKafelekWidoku(int x, int y, const std::string& nazwaObiektu);
};