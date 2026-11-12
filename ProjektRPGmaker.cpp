#include "ProjektRPGmaker.h"
#include <QGraphicsRectItem>
#include <QBrush>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QPushButton>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QFontDatabase>
#include <QFont>
#include <QWidget>
#include <QInputDialog>

#include "SystemZapisu.h"
#include "WczytajProjektDialog.h"
#include "ZapiszProjektDialog.h"
#include "StatystykiDialog.h"

#include <regex>

void ProjektRPGmaker::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        pozycjaMyszy = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void ProjektRPGmaker::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - pozycjaMyszy);
        event->accept();
    }
}

class KlikalnyKwadrat : public QGraphicsRectItem {
private:
    int gridX, gridY;
    ZarzadcaMapy* silnik;

public:
    KlikalnyKwadrat(int x, int y, int rozmiar, ZarzadcaMapy* s) : QGraphicsRectItem(x* rozmiar, y* rozmiar, rozmiar, rozmiar), gridX(x), gridY(y), silnik(s) {}

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        if (silnik->pobierzPedzel() == TypPedzla::Trawa) {
            silnik->postawObiekt(gridX, gridY, std::make_unique<Trawa>());
            this->setBrush(QBrush(QPixmap(":/sprites/trawa.png")));
        }
        else if (silnik->pobierzPedzel() == TypPedzla::Sciana) {
            silnik->postawObiekt(gridX, gridY, std::make_unique<Sciana>());
            this->setBrush(QBrush(QPixmap(":/sprites/sciana.png")));
        }
        else if (silnik->pobierzPedzel() == TypPedzla::Woda) {
            silnik->postawObiekt(gridX, gridY, std::make_unique<Woda>());
            this->setBrush(QBrush(QPixmap(":/sprites/woda.png")));
        }
        else if (silnik->pobierzPedzel() == TypPedzla::Ziemia) {
            silnik->postawObiekt(gridX, gridY, std::make_unique<Ziemia>());
            this->setBrush(QBrush(QPixmap(":/sprites/ziemia.png")));
        }
        else if (silnik->pobierzPedzel() == TypPedzla::Chodnik) {
            silnik->postawObiekt(gridX, gridY, std::make_unique<KamiennaPodloga>());
            this->setBrush(QBrush(QPixmap(":/sprites/sciezka-good.png")));
        }
        else if (silnik->pobierzPedzel() == TypPedzla::Drzwi) {
            silnik->postawObiekt(gridX, gridY, std::make_unique<Drzwi>());
            this->setBrush(QBrush(QPixmap(":/sprites/drzwi.png")));
        }
    }

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            if (silnik->przelaczStanDrzwi(gridX, gridY)) {
                char nowySymbol = silnik->pobierzSymbolZMapy(gridX, gridY);

                if (nowySymbol == 'd') {
                    this->setBrush(QBrush(QPixmap(":/sprites/drzwi-otwarte.png")));
                }
                else if (nowySymbol == 'D') {
                    this->setBrush(QBrush(QPixmap(":/sprites/drzwi.png")));
                }
            }
            event->accept();
        }
    }
};

bool ProjektRPGmaker::wczytajKonkretnyProjekt(const std::string& nazwa) {
    namespace fs = std::filesystem;
    fs::path pelnaSciezka = fs::path("ZapisaneMapy") / (nazwa + ".txt");

    ZarzadcaMapy tempEdytor;

    if (!SystemZapisu::wczytajZPliku(pelnaSciezka.string(), tempEdytor)) {
        return false; 
    }

    edytor.przejmijDane(tempEdytor);
    edytor.ustawNazweMapy(nazwa);

    scena->clear();

    int rozmiarKratki = 32;
    for (int x = 0; x < edytor.pobierzSzerokosc(); ++x) {
        for (int y = 0; y < edytor.pobierzWysokosc(); ++y) {
            KlikalnyKwadrat* kwadrat = new KlikalnyKwadrat(x, y, rozmiarKratki, &edytor);
            scena->addItem(kwadrat);

            char symbol = edytor.pobierzSymbolZMapy(x, y);
            if (symbol == '.') kwadrat->setBrush(QBrush(QPixmap(":/sprites/trawa.png")));
            else if (symbol == '#') kwadrat->setBrush(QBrush(QPixmap(":/sprites/sciana.png")));
            else if (symbol == '~') kwadrat->setBrush(QBrush(QPixmap(":/sprites/woda.png")));
            else if (symbol == ',') kwadrat->setBrush(QBrush(QPixmap(":/sprites/ziemia.png")));
            else if (symbol == '=') kwadrat->setBrush(QBrush(QPixmap(":/sprites/sciezka-good.png")));
            else if (symbol == 'D') kwadrat->setBrush(QBrush(QPixmap(":/sprites/drzwi.png")));
            else if (symbol == 'd') kwadrat->setBrush(QBrush(QPixmap(":/sprites/drzwi-otwarte.png")));
        }
    }

    scena->setSceneRect(0, 0, edytor.pobierzSzerokosc() * rozmiarKratki, edytor.pobierzWysokosc() * rozmiarKratki);
    widok->resetTransform();
    QTimer::singleShot(50, this, [this]() {
        widok->fitInView(scena->sceneRect(), Qt::KeepAspectRatio);
        });

    return true; 
}

void ProjektRPGmaker::wczytajProjekt() {
    namespace fs = std::filesystem;
    fs::path folderGlowny = "ZapisaneMapy";
    QStringList listaMap;

    if (fs::exists(folderGlowny) && fs::is_directory(folderGlowny)) {
        for (const auto& entry : fs::directory_iterator(folderGlowny)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::string nazwaMapy = entry.path().stem().string();
                listaMap << QString::fromStdString(nazwaMapy);
            }
        }
    }

    if (listaMap.isEmpty()) {
        QMessageBox::information(this, "Brak projektow", "W katalogu ZapisaneMapy nie ma jeszcze zadnych map.");
        return;
    }

    WczytajProjektDialog dialog(listaMap, this);

    if (dialog.exec() == QDialog::Accepted) {
        QString wybranaMapa = dialog.getWybranaMapa();

        wczytajKonkretnyProjekt(wybranaMapa.toStdString());

        labelOstatniZapis->setText(QString::fromStdString("ostatni zapis: " + wybranaMapa.toStdString()));
    }
}

ProjektRPGmaker::ProjektRPGmaker(QWidget* parent) : QMainWindow(parent) {

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    this->showMaximized(); 

    int fontId = QFontDatabase::addApplicationFont(":/css/pixel.ttf");
    QString fontName = "Arial";
    if (fontId != -1) { fontName = QFontDatabase::applicationFontFamilies(fontId).at(0); }
    QFont fontMenu(fontName, 12, QFont::Bold);
    QFont fontPasek(fontName, 10, QFont::Bold);

    QWidget* glownyKontener = new QWidget(this);
    glownyKontener->setObjectName("glownyKontenerEdytora");
    setCentralWidget(glownyKontener);

    QVBoxLayout* glownyLayoutWertykalny = new QVBoxLayout(glownyKontener);
    glownyLayoutWertykalny->setContentsMargins(0, 0, 0, 0);
    glownyLayoutWertykalny->setSpacing(0);

    QWidget* ramkaGorna = new QWidget(this);
    ramkaGorna->setObjectName("ramkaGornaEdytora");
    ramkaGorna->setFixedHeight(40);

    QHBoxLayout* pasekGoraLayout = new QHBoxLayout(ramkaGorna);
    pasekGoraLayout->setContentsMargins(10, 0, 15, 0);

    QPushButton* btnZapisz = new QPushButton("ZAPISZ", this);
    btnZapisz->setObjectName("btnZapiszTop");
    btnZapisz->setFont(fontPasek);

    QPushButton* btnWczytaj = new QPushButton("WCZYTAJ", this);
    btnWczytaj->setObjectName("btnWczytajTop");
    btnWczytaj->setFont(fontPasek);

    QPushButton* btnX = new QPushButton("X", this);
    btnX->setObjectName("btnCancelX");
    btnX->setFixedSize(30, 30);
    btnX->setFont(fontMenu);

    pasekGoraLayout->addWidget(btnZapisz);
    pasekGoraLayout->addSpacing(10);
    pasekGoraLayout->addWidget(btnWczytaj);
    pasekGoraLayout->addStretch();
    pasekGoraLayout->addWidget(btnX);

    glownyLayoutWertykalny->addWidget(ramkaGorna);
    connect(btnX, &QPushButton::clicked, this, &QMainWindow::close);

    QWidget* strefaRobocza = new QWidget(this);
    QHBoxLayout* roboczyLayout = new QHBoxLayout(strefaRobocza);
    roboczyLayout->setContentsMargins(0, 0, 0, 0);
    roboczyLayout->setSpacing(0);

    QWidget* panelBoczny = new QWidget(this);
    panelBoczny->setObjectName("panelBoczny");
    panelBoczny->setFixedWidth(250);

    QVBoxLayout* panelLayout = new QVBoxLayout(panelBoczny);
    panelLayout->setContentsMargins(20, 20, 20, 20);

    QLabel* tytulPedzli = new QLabel("Wybierz pedzel", this);
    tytulPedzli->setFont(fontMenu);
    tytulPedzli->setObjectName("tytulBialy");
    panelLayout->addWidget(tytulPedzli);
    panelLayout->addSpacing(10);

    QRadioButton* radioTrawa = new QRadioButton(" Trawa", this);
    QRadioButton* radioChodnik = new QRadioButton(" Chodnik", this);
    QRadioButton* radioWoda = new QRadioButton(" Woda", this);
    QRadioButton* radioZiemia = new QRadioButton(" Ziemia", this);
    QRadioButton* radioSciana = new QRadioButton(" Sciana", this);
    QRadioButton* radioDrzwi = new QRadioButton(" Drzwi", this);

    radioTrawa->setFont(fontMenu);
    radioChodnik->setFont(fontMenu);
    radioWoda->setFont(fontMenu);
    radioZiemia->setFont(fontMenu);
    radioSciana->setFont(fontMenu);
    radioDrzwi->setFont(fontMenu);

    panelLayout->addWidget(radioTrawa);
    panelLayout->addWidget(radioChodnik);
    panelLayout->addWidget(radioWoda);
    panelLayout->addWidget(radioZiemia);
    panelLayout->addSpacing(15);
    panelLayout->addWidget(radioSciana);
    panelLayout->addWidget(radioDrzwi);

    panelLayout->addStretch();

    QPushButton* btnStatystyki = new QPushButton("Wejscie w\nstatystyki", this);
    btnStatystyki->setObjectName("btnStatystyki");
    btnStatystyki->setFont(fontMenu);
    panelLayout->addWidget(btnStatystyki);

    roboczyLayout->addWidget(panelBoczny);

    scena = new QGraphicsScene(this);
    widok = new QGraphicsView(scena, this);
    widok->setObjectName("widokMapy");
    widok->setFrameShape(QFrame::NoFrame);
    roboczyLayout->addWidget(widok);

    glownyLayoutWertykalny->addWidget(strefaRobocza);

    QWidget* ramkaDolna = new QWidget(this);
    ramkaDolna->setObjectName("ramkaDolnaEdytora");
    ramkaDolna->setFixedHeight(30);

    QHBoxLayout* pasekDolLayout = new QHBoxLayout(ramkaDolna);
    pasekDolLayout->setContentsMargins(10, 0, 10, 0);

    labelOstatniZapis = new QLabel("ostatni zapis: ", this);
    labelOstatniZapis->setObjectName("tytulBialy");
    labelOstatniZapis->setFont(fontPasek);

    pasekDolLayout->addWidget(labelOstatniZapis);
    pasekDolLayout->addStretch();

    glownyLayoutWertykalny->addWidget(ramkaDolna);

    connect(radioWoda, &QRadioButton::clicked, this, [this]() { edytor.ustawPedzel(TypPedzla::Woda); });
    connect(radioZiemia, &QRadioButton::clicked, this, [this]() { edytor.ustawPedzel(TypPedzla::Ziemia); });
    connect(radioChodnik, &QRadioButton::clicked, this, [this]() { edytor.ustawPedzel(TypPedzla::Chodnik); });
    connect(radioDrzwi, &QRadioButton::clicked, this, [this]() { edytor.ustawPedzel(TypPedzla::Drzwi); });
    connect(radioTrawa, &QRadioButton::clicked, this, [this]() { edytor.ustawPedzel(TypPedzla::Trawa); });
    connect(radioSciana, &QRadioButton::clicked, this, [this]() { edytor.ustawPedzel(TypPedzla::Sciana); });

    radioSciana->setChecked(true);

    connect(btnWczytaj, &QPushButton::clicked, this, &ProjektRPGmaker::wczytajProjekt);

    connect(btnStatystyki, &QPushButton::clicked, this, [this]() {
        int iloscTerenu = edytor.policzObiektyKategorii(Kategoria::Teren);
        int iloscInfrastruktury = edytor.policzObiektyKategorii(Kategoria::Infrastruktura);

        StatystykiDialog dialog(iloscTerenu, iloscInfrastruktury, this);
        dialog.exec();
        });

    connect(btnZapisz, &QPushButton::clicked, this, [this]() {
        ZapiszProjektDialog dialog(this);

        if (dialog.exec() == QDialog::Accepted) {
            QString nazwa = dialog.getNazwa();

            if (nazwa.isEmpty()) return;

            std::string czystaNazwa = std::regex_replace(nazwa.toStdString(), std::regex(" "), "_");
            czystaNazwa = std::regex_replace(czystaNazwa, std::regex("[^a-zA-Z0-9_]"), "");

            if (czystaNazwa.empty()) czystaNazwa = "Mapa_Domyslna";

            bool sukces = SystemZapisu::zapiszDoPliku(czystaNazwa, edytor.pobierzSzerokosc(), edytor.pobierzWysokosc(), edytor.pobierzSiatke());

            if (sukces) {
                edytor.ustawNazweMapy(czystaNazwa);
                labelOstatniZapis->setText(QString::fromStdString("ostatni zapis: " + czystaNazwa));
            }
            else {
                QMessageBox::critical(this, "Blad", "Nie udalo sie zapisac pliku.");
            }
        }
        });

    int rozmiarKratki = 40;
    for (int x = 0; x < edytor.pobierzSzerokosc(); ++x) {
        for (int y = 0; y < edytor.pobierzWysokosc(); ++y) {
            KlikalnyKwadrat* kwadrat = new KlikalnyKwadrat(x, y, rozmiarKratki, &edytor);
            scena->addItem(kwadrat);

            char symbol = edytor.pobierzSymbolZMapy(x, y);
            if (symbol == '.') kwadrat->setBrush(QBrush(QPixmap(":/sprites/trawa.png").scaled(rozmiarKratki, rozmiarKratki, Qt::IgnoreAspectRatio, Qt::FastTransformation)));
            else if (symbol == '#') kwadrat->setBrush(QBrush(QPixmap(":/sprites/sciana.png").scaled(rozmiarKratki, rozmiarKratki, Qt::IgnoreAspectRatio, Qt::FastTransformation)));
            else if (symbol == '~') kwadrat->setBrush(QBrush(QPixmap(":/sprites/woda.png").scaled(rozmiarKratki, rozmiarKratki, Qt::IgnoreAspectRatio, Qt::FastTransformation)));
            else if (symbol == ',') kwadrat->setBrush(QBrush(QPixmap(":/sprites/ziemia.png").scaled(rozmiarKratki, rozmiarKratki, Qt::IgnoreAspectRatio, Qt::FastTransformation)));
            else if (symbol == '=') kwadrat->setBrush(QBrush(QPixmap(":/sprites/sciezka-good.png").scaled(rozmiarKratki, rozmiarKratki, Qt::IgnoreAspectRatio, Qt::FastTransformation)));
            else if (symbol == 'D') kwadrat->setBrush(QBrush(QPixmap(":/sprites/drzwi.png").scaled(rozmiarKratki, rozmiarKratki, Qt::IgnoreAspectRatio, Qt::FastTransformation)));
            else if (symbol == 'd') kwadrat->setBrush(QBrush(QPixmap(":/sprites/drzwi-otwarte.png").scaled(rozmiarKratki, rozmiarKratki, Qt::IgnoreAspectRatio, Qt::FastTransformation)));
        }
    }

    QString stylEdytora = R"(
        /* Górny i dolny pasek - Fioletowo/Niebieskie (Zast¹p swoim gradientem) */
        QWidget#ramkaGornaEdytora, QWidget#ramkaDolnaEdytora {
            background-color: #5C61FF; 
        }
        
        /* Zaokr¹glone górne rogi okna dla paska */
        QWidget#glownyKontenerEdytora {
            background-color: transparent;
            border: 2px solid #5C61FF;
        }

        /* Lewy panel boczny z t³em (Wpisz nazwê w³asnego t³a) */
        QWidget#panelBoczny {
            background-image: url(:/css/tlo-podprogramy.png);
            background-repeat: repeat;
            background-position: top left;
        }
        
        /* T³o obszaru mapy (Zostaje domyœlnie ciemne jak pod³o¿e w Unity/Godocie) */
        QGraphicsView#widokMapy {
            background-color: #1e1e1e;
        }

        QLabel#tytulBialy {
            color: white;
        }

        /* --- MAGIA! ZAMIANA QRadioButton w PUSTE KWADRATY (Checkboxy) --- */
        QRadioButton {
            color: white;
            spacing: 8px; /* Odstêp miêdzy polem a napisem */
        }
        
        /* Kwadracik z bia³¹ ramk¹ (Pusty przed zaznaczeniem) */
        QRadioButton::indicator {
            width: 14px;
            height: 14px;
            border: 2px solid white;
            background-color: transparent;
        }

        /* Wype³nienie kwadracika na bia³o, kiedy jest aktywny (jak zaznaczony Checkbox) */
        QRadioButton::indicator:checked {
            background-color: white;
        }

        /* Przyciski w lewym panelu (Statystyki) */
        QPushButton#btnStatystyki {
            color: white;
            text-align: left;
            background: transparent;
            border: none;
        }

        /* Przyciski tekstowe u góry (ZAPISZ, WCZYTAJ, X) */
        QPushButton#btnZapiszTop, QPushButton#btnWczytajTop, QPushButton#btnCancelX {
            color: white;
            background: transparent;
            border: none;
        }
        QPushButton#btnZapiszTop:hover, QPushButton#btnWczytajTop:hover {
            color: #EAA5F5;
        }
        QPushButton#btnCancelX:hover {
            background-color: #ef4444; /* Czerwony X po klikniêciu */
        }
    )";

    this->setStyleSheet(stylEdytora);
    edytor.ustawAkcjePoAutozapisie([this]() {
        QMetaObject::invokeMethod(this, [this]() {
            QString czasFormatowany = QTime::currentTime().toString("HH:mm:ss");
            labelOstatniZapis->setText("ostatni autozapis: " + czasFormatowany);
            });
        });
}

void ProjektRPGmaker::stworzNowyProjekt(std::string nazwa, int szer, int wys, QString wypelnienie) {
    edytor.zmienRozmiar(szer, wys);
    edytor.ustawNazweMapy(nazwa);

    for (int x = 0; x < szer; ++x) {
        for (int y = 0; y < wys; ++y) {
            if (wypelnienie == "Woda") {
                edytor.postawObiekt(x, y, std::make_unique<Woda>());
            }
            else if (wypelnienie == "Ziemia") {
                edytor.postawObiekt(x, y, std::make_unique<Ziemia>());
            }
            else if (wypelnienie == "Chodnik") {
                edytor.postawObiekt(x, y, std::make_unique<KamiennaPodloga>());
            }
            else if (wypelnienie == "Sciana") {
                edytor.postawObiekt(x, y, std::make_unique<Sciana>());
            }
            else {
                edytor.postawObiekt(x, y, std::make_unique<Trawa>());
            }
        }
    }

    scena->clear();
    int rozmiarKratki = 32; 
    for (int x = 0; x < edytor.pobierzSzerokosc(); ++x) {
        for (int y = 0; y < edytor.pobierzWysokosc(); ++y) {
            KlikalnyKwadrat* kwadrat = new KlikalnyKwadrat(x, y, rozmiarKratki, &edytor);
            scena->addItem(kwadrat);

            char symbol = edytor.pobierzSymbolZMapy(x, y);
            if (symbol == '.') kwadrat->setBrush(QBrush(QPixmap(":/sprites/trawa.png")));
            else if (symbol == '#') kwadrat->setBrush(QBrush(QPixmap(":/sprites/sciana.png")));
            else if (symbol == '~') kwadrat->setBrush(QBrush(QPixmap(":/sprites/woda.png")));
            else if (symbol == ',') kwadrat->setBrush(QBrush(QPixmap(":/sprites/ziemia.png")));
            else if (symbol == '=') kwadrat->setBrush(QBrush(QPixmap(":/sprites/sciezka-good.png")));
            else if (symbol == 'D') kwadrat->setBrush(QBrush(QPixmap(":/sprites/drzwi.png")));
            else if (symbol == 'd') kwadrat->setBrush(QBrush(QPixmap(":/sprites/drzwi-otwarte.png")));
        }
    }

    scena->setSceneRect(0, 0, edytor.pobierzSzerokosc() * rozmiarKratki, edytor.pobierzWysokosc() * rozmiarKratki);
    widok->resetTransform();
    QTimer::singleShot(50, this, [this]() {
        widok->fitInView(scena->sceneRect(), Qt::KeepAspectRatio);
        });
}

ProjektRPGmaker::~ProjektRPGmaker() {}