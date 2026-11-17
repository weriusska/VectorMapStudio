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

    }

    QString getNazwa() const { return editNazwa->text(); }
    int getSzerokosc() const { return editSzerokosc->text().toInt(); }
    int getWysokosc() const { return editWysokosc->text().toInt(); }
    QString getWypelnienie() const { return comboWypelnienie->currentText(); }
};