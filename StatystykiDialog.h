#pragma once
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QFontDatabase>
#include <QString>

class StatystykiDialog : public QDialog {
    Q_OBJECT
private:
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
    StatystykiDialog(int teren, int infra, QWidget* parent = nullptr) : QDialog(parent) {
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(450, 350);

        int fontId = QFontDatabase::addApplicationFont(":/css/pixel.ttf");
        QString fontName = "Arial";
        if (fontId != -1) { fontName = QFontDatabase::applicationFontFamilies(fontId).at(0); }

        QFont fontTytul(fontName, 16, QFont::Bold);
        QFont fontPasek(fontName, 11, QFont::Bold);
        QFont fontTekst(fontName, 12);
        QFont fontBtn(fontName, 14, QFont::Bold);

        QVBoxLayout* glownyLayout = new QVBoxLayout(this);
        glownyLayout->setContentsMargins(0, 0, 0, 0);
        glownyLayout->setSpacing(0);

        QWidget* ramkaPaska = new QWidget(this);
        ramkaPaska->setObjectName("ramkaPaskaDialog");
        ramkaPaska->setFixedSize(450, 40);

        QHBoxLayout* pasekLayout = new QHBoxLayout(ramkaPaska);
        pasekLayout->setContentsMargins(15, 0, 15, 0);

        QLabel* tytulDialog = new QLabel("STATYSTYKI MAPY", this);
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

        QLabel* labelNaglowek = new QLabel("Analiza aktualnej mapy:", this);
        labelNaglowek->setFont(fontTytul);
        labelNaglowek->setAlignment(Qt::AlignCenter);

        QLabel* labelTeren = new QLabel(QString("Obiekty terenu: %1").arg(teren), this);
        labelTeren->setFont(fontTekst);
        labelTeren->setAlignment(Qt::AlignCenter);

        QLabel* labelInfra = new QLabel(QString("Infrastruktura: %1").arg(infra), this);
        labelInfra->setFont(fontTekst);
        labelInfra->setAlignment(Qt::AlignCenter);

        QLabel* labelSuma = new QLabel(QString("Lacznie wypelnionych pol: %1").arg(teren + infra), this);
        labelSuma->setFont(fontTekst);
        labelSuma->setAlignment(Qt::AlignCenter);

        QPushButton* btnOK = new QPushButton("OK", this);
        btnOK->setObjectName("btnGotowe");
        btnOK->setFixedSize(200, 50);
        btnOK->setFont(fontBtn);

        connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);


        formularzLayout->addWidget(labelNaglowek);
        formularzLayout->addSpacing(15);
        formularzLayout->addWidget(labelTeren);
        formularzLayout->addWidget(labelInfra);
        formularzLayout->addSpacing(15);
        formularzLayout->addWidget(labelSuma);
        formularzLayout->addStretch();
        formularzLayout->addWidget(btnOK, 0, Qt::AlignCenter);

    }
};