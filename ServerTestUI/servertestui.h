#ifndef SERVERTESTUI_H
#define SERVERTESTUI_H

#include <QtWidgets/QMainWindow>
#include "ui_servertestui.h"
#include <RemoteCommunicationLibrary.h>
#include <spdlog\logger.h>

using namespace RW;

class QPushButton;
class QBoxLayout;
class QLineEdit;

class ServerTestUI : public QMainWindow
{
    Q_OBJECT
private:
    RW::COM::CommunicatonServer* m_CommunicationServer;
    QPushButton* m_NotiferTest = nullptr;
    std::shared_ptr<spdlog::logger> m_logger;
    QBoxLayout *m_MainLayout = nullptr;

    QBoxLayout *m_NotifyLayout = nullptr;
    QLineEdit *m_NotifyMessage = nullptr;

    QPushButton* m_Anel1 = nullptr;
    QPushButton* m_Anel2 = nullptr;
    QPushButton* m_Anel3 = nullptr;
    QBoxLayout *m_AnelLayout = nullptr;

    QPushButton* m_PowerSupply = nullptr;
    QPushButton* m_KL30 = nullptr;
    QBoxLayout *m_PowerSupplyLayout = nullptr;

    QBoxLayout *m_USBLayout0= nullptr;
    QBoxLayout *m_USBLayout1 = nullptr;
    QBoxLayout *m_USBLayout2 = nullptr;
    QPushButton* m_USB0 = nullptr;
    QPushButton* m_USB1_PORT0 = nullptr;
    QPushButton* m_USB1_PORT1 = nullptr;
    QPushButton* m_USB1_PORT2 = nullptr;
    QPushButton* m_USB1_PORT3 = nullptr;
    QPushButton* m_USB2_PORT0 = nullptr;
    QPushButton* m_USB2_PORT1 = nullptr;
    QPushButton* m_USB2_PORT2 = nullptr;
    QPushButton* m_USB2_PORT3 = nullptr;
    
    QPushButton* m_Reconnect = nullptr;
    QPushButton* m_Logout = nullptr;
    QBoxLayout *m_Generic = nullptr;
    QComboBox *m_ComboBox = nullptr;

public:
    ServerTestUI(QWidget *parent = 0);
    ~ServerTestUI();

private:
    Ui::ServerTestUIClass ui;

public slots:
    void OnNotify();
    void OnAnel1();
    void OnAnel2();
    void OnAnel3();
    void OnReconnect();
    void OnLogOutUser();

    void OnProcessMessage(COM::Message Msg);
signals:
    void NewMessage(COM::Message Msg);
};

#endif // SERVERTESTUI_H
