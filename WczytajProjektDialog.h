#pragma once
#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QFontDatabase>
#include <QStringList>

class WczytajProjektDialog : public QDialog {
    Q_OBJECT
private:
    QComboBox* comboMapy;
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
    WczytajProjektDialog(const QStringList& listaMap, QWidget* parent = nullptr) : QDialog(parent) {
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(500, 350); 

        int fontId = QFontDatabase::addApplicationFont(":/css/pixel.ttf");
        QString fontName = "Arial";
        if (fontId != -1) { fontName = QFontDatabase::applicationFontFamilies(fontId).at(0); }

        QFont fontNapis(fontName, 32, QFont::Bold); 
        QFont fontPasek(fontName, 16, QFont::Bold);
        QFont fontInput(fontName, 16);
        QFont fontBtn(fontName, 16, QFont::Bold);


        QVBoxLayout* glownyLayout = new QVBoxLayout(this);
        glownyLayout->setContentsMargins(0, 0, 0, 0);
        glownyLayout->setSpacing(0);

        QWidget* ramkaPaska = new QWidget(this);
        ramkaPaska->setObjectName("ramkaPaskaDialog");
        ramkaPaska->setFixedSize(500, 40);

        QHBoxLayout* pasekLayout = new QHBoxLayout(ramkaPaska);
        pasekLayout->setContentsMargins(15, 0, 15, 0);

        QLabel* tytulDialog = new QLabel("WCZYTAJ PROJEKT", this);
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
        formularzLayout->setContentsMargins(40, 20, 40, 30);
        formularzLayout->setAlignment(Qt::AlignCenter); 

        QLabel* labelWybierz = new QLabel("Wybierz mape", this);
        labelWybierz->setFont(fontNapis);
        labelWybierz->setAlignment(Qt::AlignCenter);

        comboMapy = new QComboBox(this);
        comboMapy->addItems(listaMap);
        comboMapy->setFont(fontInput);
        comboMapy->setFixedSize(350, 50); 

        btnGotowe = new QPushButton("GOTOWE", this);
        btnGotowe->setObjectName("btnGotowe");
        btnGotowe->setFixedSize(250, 60);
        btnGotowe->setFont(fontBtn);

        connect(btnGotowe, &QPushButton::clicked, this, &QDialog::accept);

        formularzLayout->addWidget(labelWybierz, 0, Qt::AlignCenter);
        formularzLayout->addSpacing(30);
        formularzLayout->addWidget(comboMapy, 0, Qt::AlignCenter);
        formularzLayout->addSpacing(40);
        formularzLayout->addWidget(btnGotowe, 0, Qt::AlignCenter);

        
    }

    QString getWybranaMapa() const {
        return comboMapy->currentText();
    }
};