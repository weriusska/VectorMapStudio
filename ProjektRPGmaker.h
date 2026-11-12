#pragma once
#include <QtWidgets/QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPoint>
#include <QLabel>
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

    QGraphicsScene* scena;
    QGraphicsView* widok;
    ZarzadcaMapy edytor{ 20, 20 };

    QPoint pozycjaMyszy;
    QLabel* labelOstatniZapis; 
};