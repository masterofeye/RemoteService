/********************************************************************************
** Form generated from reading UI file 'servertestui.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERTESTUI_H
#define UI_SERVERTESTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerTestUIClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ServerTestUIClass)
    {
        if (ServerTestUIClass->objectName().isEmpty())
            ServerTestUIClass->setObjectName(QStringLiteral("ServerTestUIClass"));
        ServerTestUIClass->resize(600, 400);
        menuBar = new QMenuBar(ServerTestUIClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        ServerTestUIClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ServerTestUIClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        ServerTestUIClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(ServerTestUIClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        ServerTestUIClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ServerTestUIClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ServerTestUIClass->setStatusBar(statusBar);

        retranslateUi(ServerTestUIClass);

        QMetaObject::connectSlotsByName(ServerTestUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *ServerTestUIClass)
    {
        ServerTestUIClass->setWindowTitle(QApplication::translate("ServerTestUIClass", "ServerTestUI", 0));
    } // retranslateUi

};

namespace Ui {
    class ServerTestUIClass: public Ui_ServerTestUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERTESTUI_H
