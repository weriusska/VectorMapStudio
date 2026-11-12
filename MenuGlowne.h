#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMouseEvent>
#include <QInputDialog>
#include <QFontDatabase>
#include <QLabel>
#include <QFont>
#include <QMessageBox>

#include <regex>
#include <filesystem>

#include "ProjektRPGmaker.h" 
#include "NowyProjektDialog.h"
#include "WczytajProjektDialog.h"

class MenuGlowne : public QMainWindow {
private:
    ProjektRPGmaker* oknoEdytora; 
    QPoint pozycjaMyszy;

protected:
    //Przesuniecie calego okna
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            pozycjaMyszy = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        }
    }

    //Przesuniecie calego okna
    void mouseMoveEvent(QMouseEvent* event) override {
        if (event->buttons() & Qt::LeftButton) {
            move(event->globalPosition().toPoint() - pozycjaMyszy);
            event->accept();
        }
    }

public:
    MenuGlowne(QWidget* parent = nullptr) : QMainWindow(parent), oknoEdytora(nullptr) {
        setWindowTitle("Menu Startowe - Kreator Map");
        resize(1024, 768);
        this->setObjectName("MojeMenu"); 
        this->setStyleSheet("#MojeMenu { border-image: url(:/css/tlo.png) 0 0 0 0 stretch stretch; }");
        this->setWindowFlags(Qt::FramelessWindowHint);

        QWidget* centralny = new QWidget(this);
        setCentralWidget(centralny);

        QVBoxLayout* layout = new QVBoxLayout(centralny);
        layout->setContentsMargins(0, 0, 0, 0);

        
        int fontId = QFontDatabase::addApplicationFont(":/css/pixel.ttf");
        QString fontName = "Arial"; 
        if (fontId != -1) {
            fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
        }
        QFont pikselowyFont(fontName, 24, QFont::Bold);
        QFont pikselowyFontPaska(fontName, 12, QFont::Bold); 

        QWidget* ramkaPaska = new QWidget(this);
        ramkaPaska->setObjectName("ramkaPaska");

        QHBoxLayout* pasekLayout = new QHBoxLayout(ramkaPaska);
        pasekLayout->setContentsMargins(15, 8, 15, 8);

        //Tytul
        QLabel* tytulApp = new QLabel("KREATOR MAP - RPG MAKER", this);
        tytulApp->setObjectName("tytulApp");
        tytulApp->setFont(pikselowyFontPaska);

        //Zamkniecie
        QPushButton* btnX = new QPushButton("X", this);
        btnX->setObjectName("btnX");
        btnX->setFont(pikselowyFontPaska);
        btnX->setFixedSize(40, 30);

        pasekLayout->addWidget(tytulApp, 0, Qt::AlignLeft | Qt::AlignVCenter);
        pasekLayout->addStretch();
        pasekLayout->addWidget(btnX, 0, Qt::AlignRight | Qt::AlignVCenter);

       
        QString stylPaska = R"(
            QWidget#ramkaPaska {
                background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, 
                                            stop: 0 #5C61FF, stop: 1 #7F83FF);
                border-bottom: 3px solid white;
            }
            QLabel#tytulApp {
                color: white;
                background-color: transparent;
                border: 2px solid white;
                border-radius: 6px;
                padding: 3px 12px;
            }
            QPushButton#btnX {
                color: white;
                background-color: transparent;
                border: none;
                font-size: 14pt;
            }
            QPushButton#btnX:hover {
                background-color: #ef4444; 
                border-radius: 4px;
            }
            QPushButton#btnX:pressed {
                background-color: #b91c1c;
            }
        )";
        ramkaPaska->setStyleSheet(stylPaska);

        connect(btnX, &QPushButton::clicked, this, &QMainWindow::close);

        layout->addWidget(ramkaPaska);

        QPushButton* btnNowy = new QPushButton("Nowy Projekt", this);
        QPushButton* btnWczytaj = new QPushButton("Wczytaj Projekt", this);
        QPushButton* btnWyjscie = new QPushButton("Wyjscie", this);

        btnNowy->setFont(pikselowyFont);
        btnWczytaj->setFont(pikselowyFont);
        btnWyjscie->setFont(pikselowyFont);

        btnNowy->setFixedSize(600, 120);
        btnWczytaj->setFixedSize(600, 120);
        btnWyjscie->setFixedSize(600, 120);

        QString stylPrzyciskow = R"(
            QPushButton {
                background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, 
                                            stop: 0 #EAA5F5, stop: 1 #A4B2F8);
                border: 4px solid white;
                border-radius: 15px;
                color: white;
            }
            QPushButton:hover {
                background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, 
                                            stop: 0 #F5B9FF, stop: 1 #BBC5FF);
                border: 4px solid #FAFAFA;
            }
            QPushButton:pressed {
                background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, 
                                            stop: 0 #D18CEB, stop: 1 #8E9CE8);
                padding-top: 5px;
            }
        )";

        this->setStyleSheet(this->styleSheet() + stylPrzyciskow);

        layout->addSpacing(400);
        layout->addWidget(btnNowy, 0, Qt::AlignCenter);
        layout->addSpacing(15);
        layout->addWidget(btnWczytaj, 0, Qt::AlignCenter);
        layout->addSpacing(15);
        layout->addWidget(btnWyjscie, 0, Qt::AlignCenter);
        layout->addStretch();

        //logika
        connect(btnNowy, &QPushButton::clicked, this, [this]() {
            NowyProjektDialog dialog(this);

            if (dialog.exec() == QDialog::Accepted) {
                QString nazwa = dialog.getNazwa();
                int szer = dialog.getSzerokosc();
                int wys = dialog.getWysokosc();
                QString wypelnienie = dialog.getWypelnienie();

                if (!oknoEdytora) {
                    oknoEdytora = new ProjektRPGmaker();
                }

                oknoEdytora->stworzNowyProjekt(nazwa.toStdString(), szer, wys, wypelnienie);
                oknoEdytora->show();
                this->hide();
            }
            });

        connect(btnWczytaj, &QPushButton::clicked, this, [this]() {
            namespace fs = std::filesystem;
            QStringList listaMap;

            if (fs::exists("ZapisaneMapy")) {
                for (const auto& entry : fs::directory_iterator("ZapisaneMapy")) {
                    if (entry.path().extension() == ".txt") {
                        listaMap << QString::fromStdString(entry.path().stem().string());
                    }
                }
            }

            if (listaMap.isEmpty()) {
                QMessageBox::information(this, "Brak projektow", "Katalog ZapisaneMapy jest pusty.");
                return;
            }


            WczytajProjektDialog dialog(listaMap, this);

            if (dialog.exec() == QDialog::Accepted) {
                QString wybranaMapa = dialog.getWybranaMapa();

                if (!oknoEdytora) {
                    oknoEdytora = new ProjektRPGmaker();
                }


                bool sukces = oknoEdytora->wczytajKonkretnyProjekt(wybranaMapa.toStdString());

                if (sukces) {
                    this->hide();
                    oknoEdytora->show();
                }
                else {

                    QMessageBox::critical(this, "Blad pliku", "Mapa jest uszkodzona. Powracam do menu.");
                    oknoEdytora->hide(); 
                    this->show();        
                }
            }
            });

        connect(btnWyjscie, &QPushButton::clicked, this, &QMainWindow::close);
    }
};