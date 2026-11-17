#include <QtWidgets/QApplication>
#include <QFontDatabase>
#include <QFile>
#include "MenuGlowne.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 1. Globalne wczytanie czcionki pikselowej
    int fontId = QFontDatabase::addApplicationFont(":/css/pixel.ttf");
    if (fontId != -1) {
        QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
        app.setFont(QFont(fontName, 12));
    }

    // 2. Globalne wczytanie arkusza stylów QSS z systemu zasobów (.qrc)
    QFile plikStylu(":/css/styl.qss");
    if (plikStylu.open(QFile::ReadOnly | QFile::Text)) {
        QString styl = QString::fromUtf8(plikStylu.readAll());
        app.setStyleSheet(styl);
    }

    MenuGlowne menu;
    menu.showMaximized();

    return app.exec();
}