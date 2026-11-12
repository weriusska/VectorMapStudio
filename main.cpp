#include <QtWidgets/QApplication>
#include <QFontDatabase>
#include "MenuGlowne.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    int fontId = QFontDatabase::addApplicationFont(":/css/pixel.ttf");
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont pikselowyFont(fontName, 14);

    MenuGlowne menu;
    menu.showMaximized();

    return app.exec();
}
