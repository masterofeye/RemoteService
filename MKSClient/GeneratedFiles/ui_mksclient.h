/********************************************************************************
** Form generated from reading UI file 'mksclient.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MKSCLIENT_H
#define UI_MKSCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MKSClientClass
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab_IntegrationPortal;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_11;
    QPushButton *pb_Flash;
    QGroupBox *gb_Match;
    QHBoxLayout *horizontalLayout_7;
    QVBoxLayout *verticalLayout_12;
    QCheckBox *cb_Bootloader;
    QCheckBox *pb_AC;
    QCheckBox *pb_GC;
    QVBoxLayout *verticalLayout_10;
    QComboBox *cb_Project;
    QComboBox *cb_Software;
    QComboBox *cb_Part;
    QComboBox *cb_Variant;
    QListWidget *lw_Software;
    QWidget *tab_DragDrop;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *gb_DragDrop;
    QWidget *tab_File;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MKSClientClass)
    {
        if (MKSClientClass->objectName().isEmpty())
            MKSClientClass->setObjectName(QStringLiteral("MKSClientClass"));
        MKSClientClass->resize(465, 328);
        centralWidget = new QWidget(MKSClientClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab_IntegrationPortal = new QWidget();
        tab_IntegrationPortal->setObjectName(QStringLiteral("tab_IntegrationPortal"));
        verticalLayout_3 = new QVBoxLayout(tab_IntegrationPortal);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setSpacing(6);
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        pb_Flash = new QPushButton(tab_IntegrationPortal);
        pb_Flash->setObjectName(QStringLiteral("pb_Flash"));

        verticalLayout_11->addWidget(pb_Flash);

        gb_Match = new QGroupBox(tab_IntegrationPortal);
        gb_Match->setObjectName(QStringLiteral("gb_Match"));
        horizontalLayout_7 = new QHBoxLayout(gb_Match);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        verticalLayout_12 = new QVBoxLayout();
        verticalLayout_12->setSpacing(6);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        cb_Bootloader = new QCheckBox(gb_Match);
        cb_Bootloader->setObjectName(QStringLiteral("cb_Bootloader"));

        verticalLayout_12->addWidget(cb_Bootloader);

        pb_AC = new QCheckBox(gb_Match);
        pb_AC->setObjectName(QStringLiteral("pb_AC"));

        verticalLayout_12->addWidget(pb_AC);

        pb_GC = new QCheckBox(gb_Match);
        pb_GC->setObjectName(QStringLiteral("pb_GC"));

        verticalLayout_12->addWidget(pb_GC);


        horizontalLayout_7->addLayout(verticalLayout_12);


        verticalLayout_11->addWidget(gb_Match);


        horizontalLayout_2->addLayout(verticalLayout_11);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setSpacing(6);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        cb_Project = new QComboBox(tab_IntegrationPortal);
        cb_Project->setObjectName(QStringLiteral("cb_Project"));

        verticalLayout_10->addWidget(cb_Project);

        cb_Software = new QComboBox(tab_IntegrationPortal);
        cb_Software->setObjectName(QStringLiteral("cb_Software"));

        verticalLayout_10->addWidget(cb_Software);

        cb_Part = new QComboBox(tab_IntegrationPortal);
        cb_Part->setObjectName(QStringLiteral("cb_Part"));

        verticalLayout_10->addWidget(cb_Part);

        cb_Variant = new QComboBox(tab_IntegrationPortal);
        cb_Variant->setObjectName(QStringLiteral("cb_Variant"));

        verticalLayout_10->addWidget(cb_Variant);


        horizontalLayout_2->addLayout(verticalLayout_10);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(1, 3);

        verticalLayout_3->addLayout(horizontalLayout_2);

        lw_Software = new QListWidget(tab_IntegrationPortal);
        lw_Software->setObjectName(QStringLiteral("lw_Software"));

        verticalLayout_3->addWidget(lw_Software);

        verticalLayout_3->setStretch(0, 5);
        verticalLayout_3->setStretch(1, 1);
        tabWidget->addTab(tab_IntegrationPortal, QString());
        tab_DragDrop = new QWidget();
        tab_DragDrop->setObjectName(QStringLiteral("tab_DragDrop"));
        horizontalLayout_5 = new QHBoxLayout(tab_DragDrop);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        gb_DragDrop = new QGroupBox(tab_DragDrop);
        gb_DragDrop->setObjectName(QStringLiteral("gb_DragDrop"));

        horizontalLayout_5->addWidget(gb_DragDrop);

        tabWidget->addTab(tab_DragDrop, QString());
        tab_File = new QWidget();
        tab_File->setObjectName(QStringLiteral("tab_File"));
        tabWidget->addTab(tab_File, QString());

        verticalLayout->addWidget(tabWidget);


        horizontalLayout->addLayout(verticalLayout);

        MKSClientClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MKSClientClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 465, 21));
        MKSClientClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MKSClientClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MKSClientClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MKSClientClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MKSClientClass->setStatusBar(statusBar);

        retranslateUi(MKSClientClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MKSClientClass);
    } // setupUi

    void retranslateUi(QMainWindow *MKSClientClass)
    {
        MKSClientClass->setWindowTitle(QApplication::translate("MKSClientClass", "MKSClient", 0));
        pb_Flash->setText(QApplication::translate("MKSClientClass", "Flash", 0));
        gb_Match->setTitle(QApplication::translate("MKSClientClass", "Software Match?", 0));
        cb_Bootloader->setText(QApplication::translate("MKSClientClass", "Bootloader", 0));
        pb_AC->setText(QApplication::translate("MKSClientClass", "AC", 0));
        pb_GC->setText(QApplication::translate("MKSClientClass", "GC", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_IntegrationPortal), QApplication::translate("MKSClientClass", "Integration Portal", 0));
        gb_DragDrop->setTitle(QApplication::translate("MKSClientClass", "Drag .ocf or .prg file here", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_DragDrop), QApplication::translate("MKSClientClass", "Drag/Drop", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_File), QApplication::translate("MKSClientClass", "File", 0));
    } // retranslateUi

};

namespace Ui {
    class MKSClientClass: public Ui_MKSClientClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MKSCLIENT_H
