#include "servertestui.h"
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qthread.h>

#include <QBoxLayout>
#include <qlineedit.h>
#include <qcombobox.h>

ServerTestUI::ServerTestUI(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    m_logger = spdlog::basic_logger_mt("basic_logger", QApplication::applicationDirPath().toStdString() + "\\basic.txt");
    m_logger->set_level(spdlog::level::trace);


    m_CommunicationServer = new RW::COM::CommunicatonServer(false, COM::TypeofServer::ServiceTest, "Server", 1234, m_logger, nullptr);
    m_CommunicationServer->Register(this);
    m_CommunicationServer->Listen();

    m_MainLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom, this);

    m_Reconnect = new QPushButton();
    m_Reconnect->setText("Reconnect to RemoteService");
    connect(m_Reconnect, &QPushButton::clicked, this, &ServerTestUI::OnReconnect);
    m_Generic = new QBoxLayout(QBoxLayout::Direction::TopToBottom, this);
    m_Generic->addWidget(m_Reconnect);
    m_MainLayout->addLayout(m_Generic);


    m_Logout = new QPushButton();
    m_Logout->setText("LogOut User");
    connect(m_Logout, &QPushButton::clicked, this, &ServerTestUI::OnLogOutUser);
    m_Generic->addWidget(m_Logout);

    m_NotifyLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight,this);

    m_NotifyMessage = new QLineEdit();
    m_NotifyLayout->addWidget(m_NotifyMessage);

    m_NotiferTest = new QPushButton();
    m_NotiferTest->setText("Notify");
    m_ComboBox = new QComboBox();
    m_ComboBox->insertItem(0, "Information");
    m_ComboBox->insertItem(1, "Warning");
    m_ComboBox->insertItem(2, "Alert");
    connect(m_NotiferTest, &QPushButton::clicked, this, &ServerTestUI::OnNotify);
    m_NotifyLayout->addWidget(m_ComboBox);
    m_NotifyLayout->addWidget(m_NotiferTest);
    m_MainLayout->addLayout(m_NotifyLayout);

    QPushButton* m_Anel1 = new QPushButton();
    m_Anel1->setText("Anel1");
    connect(m_Anel1, &QPushButton::clicked, this, &ServerTestUI::OnAnel1);
    QPushButton* m_Anel2 = new QPushButton();
    m_Anel2->setText("Anel2");
    connect(m_Anel2, &QPushButton::clicked, this, &ServerTestUI::OnAnel2);
    QPushButton* m_Anel3 = new QPushButton();
    m_Anel3->setText("Anel3");
    connect(m_Anel3, &QPushButton::clicked, this, &ServerTestUI::OnAnel3);
    QBoxLayout *m_AnelLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);
    m_AnelLayout->addWidget(m_Anel1);
    m_AnelLayout->addWidget(m_Anel2);
    m_AnelLayout->addWidget(m_Anel3);
    m_MainLayout->addLayout(m_AnelLayout);

    QPushButton* m_PowerSupply = new QPushButton();
    m_PowerSupply->setText("PowerSupply");
    QPushButton* m_KL30 = new QPushButton();;
    m_KL30->setText("KL30");
    QBoxLayout *m_PowerSupplyLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);;
    m_PowerSupplyLayout->addWidget(m_PowerSupply);
    m_PowerSupplyLayout->addWidget(m_KL30);
    m_MainLayout->addLayout(m_PowerSupplyLayout);

    QBoxLayout *m_USBLayout0 = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);;;
    QBoxLayout *m_USBLayout1 = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);;;
    QBoxLayout *m_USBLayout2 = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);;;
    QPushButton* m_USB0 = new QPushButton();
    m_USB0->setText("USB0");
    m_USBLayout0->addWidget(m_USB0);
    m_MainLayout->addLayout(m_USBLayout0);

    QPushButton* m_USB1_PORT0 = new QPushButton();
    m_USB1_PORT0->setText("USB1_PORT0");
    QPushButton* m_USB1_PORT1 = new QPushButton();
    m_USB1_PORT1->setText("USB1_PORT1");
    QPushButton* m_USB1_PORT2 = new QPushButton();
    m_USB1_PORT2->setText("USB1_PORT2");
    QPushButton* m_USB1_PORT3 = new QPushButton();
    m_USB1_PORT3->setText("USB1_PORT3");

    m_USBLayout1->addWidget(m_USB1_PORT0);
    m_USBLayout1->addWidget(m_USB1_PORT1);
    m_USBLayout1->addWidget(m_USB1_PORT2);
    m_USBLayout1->addWidget(m_USB1_PORT3);
    m_MainLayout->addLayout(m_USBLayout1);

    QPushButton* m_USB2_PORT0 = new QPushButton();
    m_USB2_PORT0->setText("USB2_PORT0");
    QPushButton* m_USB2_PORT1 = new QPushButton();
    m_USB2_PORT1->setText("USB2_PORT1");
    QPushButton* m_USB2_PORT2 = new QPushButton();
    m_USB2_PORT2->setText("USB2_PORT2");
    QPushButton* m_USB2_PORT3 = new QPushButton();
    m_USB2_PORT3->setText("USB2_PORT3");

    m_USBLayout2->addWidget(m_USB2_PORT0);
    m_USBLayout2->addWidget(m_USB2_PORT1);
    m_USBLayout2->addWidget(m_USB2_PORT2);
    m_USBLayout2->addWidget(m_USB2_PORT3);
    m_MainLayout->addLayout(m_USBLayout2);












    this->centralWidget()->setLayout(m_MainLayout);

}

ServerTestUI::~ServerTestUI()
{

}

void ServerTestUI::OnNotify()
{
    RW::COM::Message m;
    m.SetMessageID(RW::COM::MessageDescription::IN_Notifier);
    bool isTimerout = false;
    quint64 timeout = 0;
    QString Message = "Das ist eine Testnachricht";
    QList<QVariant> parameter;
    parameter.append(isTimerout);
    parameter.append(timeout);
    parameter.append(m_NotifyMessage->text());
    parameter.append(m_ComboBox->currentIndex());
    m.SetParameterList(parameter);
    m.SetIsExternal(true);
    emit NewMessage(m);
} 

void ServerTestUI::OnAnel1()
{
    RW::COM::Message m;
    m.SetMessageID(RW::COM::MessageDescription::IN_POWERSTRIPE);
    QList<QVariant> parameter;
    bool state = true;
    int port = 1;

    parameter.append(port);
    parameter.append(state);
    m.SetParameterList(parameter);
    m.SetIsExternal(true);
    m.SetExcVariant(RW::COM::Message::ExecutionVariant::SET);
    emit NewMessage(m);
}

void ServerTestUI::OnAnel2()
{
    RW::COM::Message m;
    m.SetMessageID(RW::COM::MessageDescription::IN_POWERSTRIPE);
    QList<QVariant> parameter;
    bool state = true;
    int port = 2;

    parameter.append(port);
    parameter.append(state);
    m.SetParameterList(parameter);
    m.SetIsExternal(true);
    m.SetExcVariant(RW::COM::Message::ExecutionVariant::SET);
    emit NewMessage(m);
}

void ServerTestUI::OnAnel3()
{
    RW::COM::Message m;
    m.SetMessageID(RW::COM::MessageDescription::IN_POWERSTRIPE);
    QList<QVariant> parameter;
    bool state = true;
    int port = 3;

    parameter.append(port);
    parameter.append(state);
    m.SetParameterList(parameter);
    m.SetIsExternal(true);
    m.SetExcVariant(RW::COM::Message::ExecutionVariant::SET);
    emit NewMessage(m);
}

void ServerTestUI::OnReconnect(){
    m_CommunicationServer = new RW::COM::CommunicatonServer(false, COM::TypeofServer::ServiceTest, "Server", 1234, m_logger, nullptr);
    m_CommunicationServer->Register(this);
    m_CommunicationServer->Listen();
}

void ServerTestUI::OnProcessMessage(RW::COM::Message Msg)
{

}

void ServerTestUI::OnLogOutUser()
{
    RW::COM::Message m;
    m.SetMessageID(RW::COM::MessageDescription::IN_LogoutImmediately);
    m.SetIsExternal(true);
    m.SetExcVariant(RW::COM::Message::ExecutionVariant::NON);
    m.setIdentifier(COM::Message::GenUUID(COM::TypeofServer::ServiceTest).toString());
    emit NewMessage(m);
}