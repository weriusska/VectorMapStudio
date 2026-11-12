#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QPoint>
#include <QFontDatabase>
#include <QFont>
#include <QMessageBox>

#include <regex>

class NowyProjektDialog : public QDialog {
    Q_OBJECT
private:
    QLineEdit* editNazwa;
    QLineEdit* editSzerokosc;
    QLineEdit* editWysokosc;
    QComboBox* comboWypelnienie;
    QPushButton* btnGotowe;

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
    NowyProjektDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(500, 450);


        int fontId = QFontDatabase::addApplicationFont(":/css/pixel.ttf");
        QString fontName = "Arial"; 
        if (fontId != -1) {
            fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
        }
        QFont fontLabel(fontName, 12, QFont::Bold);
        QFont fontInput(fontName, 10);
        QFont fontPasek(fontName, 11, QFont::Bold);
        QFont fontBtn(fontName, 14, QFont::Bold);


        QVBoxLayout* glownyLayout = new QVBoxLayout(this);
        glownyLayout->setContentsMargins(0, 0, 0, 0);
        glownyLayout->setSpacing(0);

        QWidget* ramkaPaska = new QWidget(this);
        ramkaPaska->setObjectName("ramkaPaskaDialog");
        ramkaPaska->setFixedSize(500, 40);

        QHBoxLayout* pasekLayout = new QHBoxLayout(ramkaPaska);
        pasekLayout->setContentsMargins(15, 0, 15, 0);

        QLabel* tytulDialog = new QLabel("NOWY PROJEKT", this);
        tytulDialog->setObjectName("tytulDialog");
        tytulDialog->setFont(fontPasek);

        QPushButton* btnCancel = new QPushButton("X", this);
        btnCancel->setObjectName("btnCancelX");
        btnCancel->setFixedSize(30, 30);
        btnCancel->setFont(fontPasek);

        pasekLayout->addWidget(tytulDialog, 0, Qt::AlignLeft | Qt::AlignVCenter);
        pasekLayout->addStretch();
        pasekLayout->addWidget(btnCancel, 0, Qt::AlignRight | Qt::AlignVCenter);

        connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
        glownyLayout->addWidget(ramkaPaska);

        QWidget* formularzWidget = new QWidget(this);
        formularzWidget->setObjectName("formularzWidget");
        glownyLayout->addWidget(formularzWidget);

        QVBoxLayout* formularzLayout = new QVBoxLayout(formularzWidget);
        formularzLayout->setContentsMargins(40, 30, 40, 30);

        QFormLayout* formLayout = new QFormLayout();
        formLayout->setSpacing(25);
        formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QLabel* labelNazwa = new QLabel("Nazwa projektu:", this);
        labelNazwa->setFont(fontLabel);
        editNazwa = new QLineEdit(this);
        editNazwa->setFont(fontInput);

        QLabel* labelSzer = new QLabel("Szerokosc (X):", this);
        labelSzer->setFont(fontLabel);
        editSzerokosc = new QLineEdit(this);
        editSzerokosc->setFont(fontInput);

        QLabel* labelWys = new QLabel("Wysokosc (Y):", this);
        labelWys->setFont(fontLabel);
        editWysokosc = new QLineEdit(this);
        editWysokosc->setFont(fontInput);

        QLabel* labelWyp = new QLabel("Wypelnienie:", this);
        labelWyp->setFont(fontLabel);
        comboWypelnienie = new QComboBox(this);
        comboWypelnienie->addItems({ "Trawa", "Sciana", "Woda", "Ziemia", "Chodnik" });
        comboWypelnienie->setFont(fontInput);

        formLayout->addRow(labelNazwa, editNazwa);
        formLayout->addRow(labelSzer, editSzerokosc);
        formLayout->addRow(labelWys, editWysokosc);
        formLayout->addRow(labelWyp, comboWypelnienie);

        formularzLayout->addLayout(formLayout);
        formularzLayout->addStretch();

        btnGotowe = new QPushButton("GOTOWE", this);
        btnGotowe->setObjectName("btnGotowe");
        btnGotowe->setFixedSize(250, 60);
        btnGotowe->setFont(fontBtn);
        formularzLayout->addWidget(btnGotowe, 0, Qt::AlignCenter);

        connect(btnGotowe, &QPushButton::clicked, this, [this]() {
            std::string nazwa = editNazwa->text().toStdString();
            std::string szerStr = editSzerokosc->text().toStdString();
            std::string wysStr = editWysokosc->text().toStdString();


            std::regex wzorNazwy("^[a-zA-Z0-9_]{1,20}$");
            if (!std::regex_match(nazwa, wzorNazwy)) {
                QMessageBox::warning(this, "Blad walidacji", "Nazwa projektu: tylko litery, cyfry i podkreslniki (max 20 znakow).");
                return;
            }

            std::regex wzorWymiarow("^(1[0-9]{1}|[2-9][0-9]{1}|1[0-9]{2}|200)$");
            if (!std::regex_match(szerStr, wzorWymiarow) || !std::regex_match(wysStr, wzorWymiarow)) {
                QMessageBox::warning(this, "Blad walidacji", "Wymiary musza byc liczbami z zakresu 10-200.");
                return;
            }

            this->accept();
            });

        QString stylDialogu = R"(
            QWidget#ramkaPaskaDialog {
                background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, 
                                            stop: 0 #5C61FF, stop: 1 #7F83FF);
                border-top-left-radius: 15px;
                border-top-right-radius: 15px;
                border-bottom: 3px solid white;
            }
            
            QLabel#tytulDialog {
                color: white;
                background-color: transparent;
                border: 2px solid white;
                border-radius: 6px;
                padding: 3px 12px;
            }
            
            QPushButton#btnCancelX {
                color: white;
                background-color: transparent;
                border: none;
                font-size: 14pt;
            }
            QPushButton#btnCancelX:hover {
                background-color: #ef4444;
                color: white;
                border-radius: 4px;
            }
            QPushButton#btnCancelX:pressed {
                background-color: #b91c1c;
            }

            /* --- PODMIENIONA ŒCIE¯KA DO T£A --- */
            QWidget#formularzWidget {
                border-image: url(:/css/tlo-podprogramy.png) 0 0 0 0 stretch stretch;
                border-bottom-left-radius: 15px;
                border-bottom-right-radius: 15px;
                border-top: none;
            }

            QLabel {
                color: white;
            }

            QLineEdit, QComboBox {
                background-color: rgba(255, 255, 255, 0.7);
                border: 2px solid white;
                border-radius: 10px;
                padding: 5px;
                color: #3e285a;
            }
            QLineEdit:focus, QComboBox:focus {
                background-color: white;
            }

            QPushButton#btnGotowe {
                background-color: rgba(255, 255, 255, 0.3);
                border: 3px solid white;
                border-radius: 15px;
                color: white;
            }
            QPushButton#btnGotowe:hover {
                background-color: rgba(255, 255, 255, 0.6);
                border: 3px solid #FAFAFA;
            }
            QPushButton#btnGotowe:pressed {
                background-color: rgba(255, 255, 255, 0.8);
                padding-top: 5px;
            }
        )";

        this->setStyleSheet(stylDialogu);
    }

    QString getNazwa() const { return editNazwa->text(); }
    int getSzerokosc() const { return editSzerokosc->text().toInt(); }
    int getWysokosc() const { return editWysokosc->text().toInt(); }
    QString getWypelnienie() const { return comboWypelnienie->currentText(); }
};