/********************************************************************************
** Form generated from reading UI file 'ProjektRPGmaker.ui'
**
** Created by: Qt User Interface Compiler version 6.10.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROJEKTRPGMAKER_H
#define UI_PROJEKTRPGMAKER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProjektRPGmakerClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ProjektRPGmakerClass)
    {
        if (ProjektRPGmakerClass->objectName().isEmpty())
            ProjektRPGmakerClass->setObjectName("ProjektRPGmakerClass");
        ProjektRPGmakerClass->resize(600, 400);
        menuBar = new QMenuBar(ProjektRPGmakerClass);
        menuBar->setObjectName("menuBar");
        ProjektRPGmakerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ProjektRPGmakerClass);
        mainToolBar->setObjectName("mainToolBar");
        ProjektRPGmakerClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(ProjektRPGmakerClass);
        centralWidget->setObjectName("centralWidget");
        ProjektRPGmakerClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ProjektRPGmakerClass);
        statusBar->setObjectName("statusBar");
        ProjektRPGmakerClass->setStatusBar(statusBar);

        retranslateUi(ProjektRPGmakerClass);

        QMetaObject::connectSlotsByName(ProjektRPGmakerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ProjektRPGmakerClass)
    {
        ProjektRPGmakerClass->setWindowTitle(QCoreApplication::translate("ProjektRPGmakerClass", "ProjektRPGmaker", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProjektRPGmakerClass: public Ui_ProjektRPGmakerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROJEKTRPGMAKER_H
