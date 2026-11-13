#include "ProjektRPGmaker.h"
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QPushButton>
#include <QTime>
#include <QTimer>
#include <QFontDatabase>
#include <QFont>
#include <QWidget>
#include <regex>

#include "SystemZapisu.h"
#include "WczytajProjektDialog.h"
#include "ZapiszProjektDialog.h"
#include "StatystykiDialog.h"

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

// Kafelek UI jest teraz czystym kontrolerem zdarzeñ myszy (zgodnie z MVC)
class KlikalnyKwadrat : public QGraphicsRectItem {
private:
    int gridX, gridY;
    ZarzadcaMapy* silnik;

public:
    KlikalnyKwadrat(int x, int y, int rozmiar, ZarzadcaMapy* s)
        : QGraphicsRectItem(x* rozmiar, y* rozmiar, rozmiar, rozmiar),
        gridX(x), gridY(y), silnik(s) {
    }

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            silnik->namalujAktualnymPedzlem(gridX, gridY);
            event->accept();
        }
    }

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            silnik->przelaczStanDrzwi(gridX, gridY);
            event->accept();
        }
    }
};

void ProjektRPGmaker::inicjalizujSlownikTekstur() {
    auto wczytajTeksture = [](const QString& sciezka) -> QBrush {
        return QBrush(QPixmap(sciezka).scaled(
            ROZMIAR_KRATKI, ROZMIAR_KRATKI,
            Qt::IgnoreAspectRatio, Qt::FastTransformation));
        };

    slownikTekstur["Trawa"] = wczytajTeksture(":/sprites/trawa.png");
    slownikTekstur["Sciana"] = wczytajTeksture(":/sprites/sciana.png");
    slownikTekstur["Woda"] = wczytajTeksture(":/sprites/woda.png");
    slownikTekstur["Ziemia"] = wczytajTeksture(":/sprites/ziemia.png");
    slownikTekstur["KamiennaPodloga"] = wczytajTeksture(":/sprites/sciezka-good.png");
    slownikTekstur["Drzwi"] = wczytajTeksture(":/sprites/drzwi.png");
    slownikTekstur["DrzwiOtwarte"] = wczytajTeksture(":/sprites/drzwi-otwarte.png");
}

void ProjektRPGmaker::odswiezKafelekWidoku(int x, int y, const std::string& nazwaObiektu) {
    int szer = edytor.pobierzSzerokosc();
    int wys = edytor.pobierzWysokosc();
    if (x < 0 || x >= szer || y < 0 || y >= wys) return;

    size_t idx = static_cast<size_t>(y) * static_cast<size_t>(szer) + static_cast<size_t>(x);
    if (idx < kafelkiWidoku.size() && kafelkiWidoku[idx] != nullptr) {
        auto it = slownikTekstur.find(nazwaObiektu);
        if (it != slownikTekstur.end()) {
            kafelkiWidoku[idx]->setBrush(it->second);
        }
    }
}

void ProjektRPGmaker::przebudujScene() {
    scena->clear();
    int szer = edytor.pobierzSzerokosc();
    int wys = edytor.pobierzWysokosc();

    kafelkiWidoku.assign(static_cast<size_t>(szer) * static_cast<size_t>(wys), nullptr);

    for (int y = 0; y < wys; ++y) {
        for (int x = 0; x < szer; ++x) {
            KlikalnyKwadrat* kwadrat = new KlikalnyKwadrat(x, y, ROZMIAR_KRATKI, &edytor);
            scena->addItem(kwadrat);

            size_t idx = static_cast<size_t>(y) * static_cast<size_t>(szer) + static_cast<size_t>(x);
            kafelkiWidoku[idx] = kwadrat;

            std::string nazwa = edytor.pobierzNazweZMapy(x, y);
            odswiezKafelekWidoku(x, y, nazwa);
        }
    }

    scena->setSceneRect(0, 0, szer * ROZMIAR_KRATKI, wys * ROZMIAR_KRATKI);
    widok->resetTransform();
    QTimer::singleShot(50, this, [this]() {
        widok->fitInView(scena->sceneRect(), Qt::KeepAspectRatio);
        });
}

bool ProjektRPGmaker::wczytajKonkretnyProjekt(const std::string& nazwa) {
    namespace fs = std::filesystem;
    fs::path pelnaSciezka = fs::path("ZapisaneMapy") / (nazwa + ".txt");

    ZarzadcaMapy tempEdytor;

    if (!SystemZapisu::wczytajZPliku(pelnaSciezka.string(), tempEdytor)) {
        return false;
    }

    edytor.przejmijDane(tempEdytor);
    edytor.ustawNazweMapy(nazwa);

    przebudujScene();
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
        if (wczytajKonkretnyProjekt(wybranaMapa.toStdString())) {
            labelOstatniZapis->setText("ostatni zapis: " + wybranaMapa);
        }
    }
}

ProjektRPGmaker::ProjektRPGmaker(QWidget* parent) : QMainWindow(parent) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    this->showMaximized();

    inicjalizujSlownikTekstur();

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

            bool sukces = SystemZapisu::zapiszDoPliku(
                czystaNazwa, edytor.pobierzSzerokosc(), edytor.pobierzWysokosc(), edytor.pobierzSiatke());

            if (sukces) {
                edytor.ustawNazweMapy(czystaNazwa);
                labelOstatniZapis->setText(QString::fromStdString("ostatni zapis: " + czystaNazwa));
            }
            else {
                QMessageBox::critical(this, "Blad", "Nie udalo sie zapisac pliku.");
            }
        }
        });

    // Podpiêcie wzorca Observer (MVC): ka¿da zmiana w modelu automatycznie odœwie¿a kafelek w UI
    edytor.ustawObserwatoraMapy([this](int x, int y, const std::string& nazwaObiektu) {
        odswiezKafelekWidoku(x, y, nazwaObiektu);
        });

    edytor.ustawAkcjePoAutozapisie([this]() {
        QMetaObject::invokeMethod(this, [this]() {
            QString czasFormatowany = QTime::currentTime().toString("HH:mm:ss");
            labelOstatniZapis->setText("ostatni autozapis: " + czasFormatowany);
            });
        });

    przebudujScene();

    QString stylEdytora = R"(
        QWidget#ramkaGornaEdytora, QWidget#ramkaDolnaEdytora {
            background-color: #5C61FF; 
        }
        QWidget#glownyKontenerEdytora {
            background-color: transparent;
            border: 2px solid #5C61FF;
        }
        QWidget#panelBoczny {
            background-image: url(:/css/tlo-podprogramy.png);
            background-repeat: repeat;
            background-position: top left;
        }
        QGraphicsView#widokMapy {
            background-color: #1e1e1e;
        }
        QLabel#tytulBialy {
            color: white;
        }
        QRadioButton {
            color: white;
            spacing: 8px;
        }
        QRadioButton::indicator {
            width: 14px;
            height: 14px;
            border: 2px solid white;
            background-color: transparent;
        }
        QRadioButton::indicator:checked {
            background-color: white;
        }
        QPushButton#btnStatystyki {
            color: white;
            text-align: left;
            background: transparent;
            border: none;
        }
        QPushButton#btnZapiszTop, QPushButton#btnWczytajTop, QPushButton#btnCancelX {
            color: white;
            background: transparent;
            border: none;
        }
        QPushButton#btnZapiszTop:hover, QPushButton#btnWczytajTop:hover {
            color: #EAA5F5;
        }
        QPushButton#btnCancelX:hover {
            background-color: #ef4444;
        }
    )";

    this->setStyleSheet(stylEdytora);
}

void ProjektRPGmaker::stworzNowyProjekt(std::string nazwa, int szer, int wys, QString wypelnienie) {
    // Tymczasowo odpinamy obserwatora na czas masowego wype³niania nowej mapy
    edytor.ustawObserwatoraMapy(nullptr);

    edytor.zmienRozmiar(szer, wys);
    edytor.ustawNazweMapy(nazwa);

    for (int y = 0; y < wys; ++y) {
        for (int x = 0; x < szer; ++x) {
            if (wypelnienie == "Woda")         edytor.postawObiekt(x, y, std::make_unique<Woda>());
            else if (wypelnienie == "Ziemia")  edytor.postawObiekt(x, y, std::make_unique<Ziemia>());
            else if (wypelnienie == "Chodnik") edytor.postawObiekt(x, y, std::make_unique<KamiennaPodloga>());
            else if (wypelnienie == "Sciana")  edytor.postawObiekt(x, y, std::make_unique<Sciana>());
            else                               edytor.postawObiekt(x, y, std::make_unique<Trawa>());
        }
    }

    przebudujScene();

    // Ponownie podpinamy obserwatora po zbudowaniu sceny
    edytor.ustawObserwatoraMapy([this](int x, int y, const std::string& nazwaObiektu) {
        odswiezKafelekWidoku(x, y, nazwaObiektu);
        });
}

ProjektRPGmaker::~ProjektRPGmaker() {}