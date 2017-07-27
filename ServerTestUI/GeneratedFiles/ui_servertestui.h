/********************************************************************************
** Form generated from reading UI file 'servertestui.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERTESTUI_H
#define UI_SERVERTESTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerTestUIClass
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTabWidget *Tab;
    QWidget *CommunicationTest;
    QWidget *DatabaseTest_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ServerTestUIClass)
    {
        if (ServerTestUIClass->objectName().isEmpty())
            ServerTestUIClass->setObjectName(QStringLiteral("ServerTestUIClass"));
        ServerTestUIClass->resize(600, 400);
        centralWidget = new QWidget(ServerTestUIClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        Tab = new QTabWidget(centralWidget);
        Tab->setObjectName(QStringLiteral("Tab"));
        CommunicationTest = new QWidget();
        CommunicationTest->setObjectName(QStringLiteral("CommunicationTest"));
        Tab->addTab(CommunicationTest, QString());
        DatabaseTest_2 = new QWidget();
        DatabaseTest_2->setObjectName(QStringLiteral("DatabaseTest_2"));
        Tab->addTab(DatabaseTest_2, QString());

        horizontalLayout->addWidget(Tab);

        ServerTestUIClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ServerTestUIClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        ServerTestUIClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ServerTestUIClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        ServerTestUIClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ServerTestUIClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ServerTestUIClass->setStatusBar(statusBar);

        retranslateUi(ServerTestUIClass);

        Tab->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(ServerTestUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *ServerTestUIClass)
    {
        ServerTestUIClass->setWindowTitle(QApplication::translate("ServerTestUIClass", "ServerTestUI", Q_NULLPTR));
        Tab->setTabText(Tab->indexOf(CommunicationTest), QApplication::translate("ServerTestUIClass", "Tab 1", Q_NULLPTR));
        Tab->setTabText(Tab->indexOf(DatabaseTest_2), QApplication::translate("ServerTestUIClass", "Tab 2", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ServerTestUIClass: public Ui_ServerTestUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERTESTUI_H
