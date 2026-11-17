#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMouseEvent>
#include <QFontDatabase>
#include <QLabel>
#include <QFont>
#include <QMessageBox>
#include <filesystem>

#include "ProjektRPGmaker.h" 
#include "NowyProjektDialog.h"
#include "WczytajProjektDialog.h"

class MenuGlowne : public QMainWindow {
private:
    ProjektRPGmaker* oknoEdytora;
    QPoint pozycjaMyszy;

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            pozycjaMyszy = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (event->buttons() & Qt::LeftButton) {
            move(event->globalPosition().toPoint() - pozycjaMyszy);
            event->accept();
        }
    }

public:
    MenuGlowne(QWidget* parent = nullptr) : QMainWindow(parent), oknoEdytora(nullptr) {
        setWindowTitle("VectorMap Studio - Menu Startowe");
        resize(1024, 768);
        this->setObjectName("MojeMenu");
        this->setWindowFlags(Qt::FramelessWindowHint);

        QWidget* centralny = new QWidget(this);
        setCentralWidget(centralny);

        QVBoxLayout* layout = new QVBoxLayout(centralny);
        layout->setContentsMargins(0, 0, 0, 0);

        QString fontName = font().family();
        QFont pikselowyFont(fontName, 24, QFont::Bold);
        QFont pikselowyFontPaska(fontName, 12, QFont::Bold);

        QWidget* ramkaPaska = new QWidget(this);
        ramkaPaska->setObjectName("ramkaPaska");

        QHBoxLayout* pasekLayout = new QHBoxLayout(ramkaPaska);
        pasekLayout->setContentsMargins(15, 8, 15, 8);

        QLabel* tytulApp = new QLabel("VECTORMAP STUDIO", this);
        tytulApp->setObjectName("tytulApp");
        tytulApp->setFont(pikselowyFontPaska);

        QPushButton* btnX = new QPushButton("X", this);
        btnX->setObjectName("btnX");
        btnX->setFont(pikselowyFontPaska);
        btnX->setFixedSize(40, 30);

        pasekLayout->addWidget(tytulApp, 0, Qt::AlignLeft | Qt::AlignVCenter);
        pasekLayout->addStretch();
        pasekLayout->addWidget(btnX, 0, Qt::AlignRight | Qt::AlignVCenter);

        connect(btnX, &QPushButton::clicked, this, &QMainWindow::close);
        layout->addWidget(ramkaPaska);

        QPushButton* btnNowy = new QPushButton("Nowy Projekt", this);
        QPushButton* btnWczytaj = new QPushButton("Wczytaj Projekt", this);
        QPushButton* btnWyjscie = new QPushButton("Wyjscie", this);

        btnNowy->setObjectName("btnMenuGlowne");
        btnWczytaj->setObjectName("btnMenuGlowne");
        btnWyjscie->setObjectName("btnMenuGlowne");

        btnNowy->setFont(pikselowyFont);
        btnWczytaj->setFont(pikselowyFont);
        btnWyjscie->setFont(pikselowyFont);

        btnNowy->setFixedSize(600, 120);
        btnWczytaj->setFixedSize(600, 120);
        btnWyjscie->setFixedSize(600, 120);

        layout->addSpacing(400);
        layout->addWidget(btnNowy, 0, Qt::AlignCenter);
        layout->addSpacing(15);
        layout->addWidget(btnWczytaj, 0, Qt::AlignCenter);
        layout->addSpacing(15);
        layout->addWidget(btnWyjscie, 0, Qt::AlignCenter);
        layout->addStretch();

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

                if (oknoEdytora->wczytajKonkretnyProjekt(wybranaMapa.toStdString())) {
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