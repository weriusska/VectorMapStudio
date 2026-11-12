#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QFontDatabase>

class ZapiszProjektDialog : public QDialog {
    Q_OBJECT
private:
    QLineEdit* editNazwa;
    QPushButton* btnZapisz;
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
    ZapiszProjektDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(450, 300); 

        int fontId = QFontDatabase::addApplicationFont(":/css/pixel.ttf");
        QString fontName = "Arial";
        if (fontId != -1) { fontName = QFontDatabase::applicationFontFamilies(fontId).at(0); }

        QFont fontTytul(fontName, 18, QFont::Bold);
        QFont fontPasek(fontName, 11, QFont::Bold);
        QFont fontInput(fontName, 12);
        QFont fontBtn(fontName, 14, QFont::Bold);

        QVBoxLayout* glownyLayout = new QVBoxLayout(this);
        glownyLayout->setContentsMargins(0, 0, 0, 0);
        glownyLayout->setSpacing(0);

        QWidget* ramkaPaska = new QWidget(this);
        ramkaPaska->setObjectName("ramkaPaskaDialog");
        ramkaPaska->setFixedSize(450, 40);

        QHBoxLayout* pasekLayout = new QHBoxLayout(ramkaPaska);
        pasekLayout->setContentsMargins(15, 0, 15, 0);

        QLabel* tytulDialog = new QLabel("ZAPISZ PROJEKT", this);
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
        formularzLayout->setAlignment(Qt::AlignCenter);

        QLabel* labelPodaj = new QLabel("Podaj nazwe mapy:", this);
        labelPodaj->setFont(fontTytul);
        labelPodaj->setAlignment(Qt::AlignCenter);

        editNazwa = new QLineEdit(this);
        editNazwa->setFont(fontInput);
        editNazwa->setPlaceholderText("Moja_Fajna_Mapa");
        editNazwa->setFixedSize(350, 40);
        editNazwa->setAlignment(Qt::AlignCenter);

        btnZapisz = new QPushButton("ZAPISZ", this);
        btnZapisz->setObjectName("btnGotowe");
        btnZapisz->setFixedSize(200, 50);
        btnZapisz->setFont(fontBtn);

        connect(btnZapisz, &QPushButton::clicked, this, &QDialog::accept);

        formularzLayout->addWidget(labelPodaj, 0, Qt::AlignCenter);
        formularzLayout->addSpacing(20);
        formularzLayout->addWidget(editNazwa, 0, Qt::AlignCenter);
        formularzLayout->addSpacing(30);
        formularzLayout->addWidget(btnZapisz, 0, Qt::AlignCenter);

        QString stylDialogu = R"(
            QWidget#ramkaPaskaDialog {
                background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, 
                                            stop: 0 #5C61FF, stop: 1 #7F83FF);
                border-top-left-radius: 15px;
                border-top-right-radius: 15px;
                border-bottom: 3px solid white;
            }
            QLabel#tytulDialog {
                color: white; background-color: transparent; border: 2px solid white; 
                border-radius: 6px; padding: 3px 12px;
            }
            QPushButton#btnCancelX {
                color: white; background-color: transparent; border: none; font-size: 14pt;
            }
            QPushButton#btnCancelX:hover {
                background-color: #ef4444; color: white; border-radius: 4px;
            }
            QPushButton#btnCancelX:pressed { background-color: #b91c1c; }

            /* --- PODMIENIONA ŒCIE¯KA DO T£A --- */
            QWidget#formularzWidget {
                border-image: url(:/css/tlo-podprogramy.png) 0 0 0 0 stretch stretch;
                border-bottom-left-radius: 15px;
                border-bottom-right-radius: 15px;
                border-top: none;
            }

            QLabel { color: white; }

            QLineEdit {
                background-color: rgba(255, 255, 255, 0.7);
                border: 2px solid white;
                border-radius: 10px;
                padding: 5px;
                color: #3e285a;
            }
            QLineEdit:focus { background-color: white; }

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
};