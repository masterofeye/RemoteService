#include "servertestui.h"
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qthread.h>

ServerTestUI::ServerTestUI(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    m_logger = spdlog::basic_logger_mt("basic_logger", QApplication::applicationDirPath().toStdString() + "\\basic.txt");
    m_logger->set_level(spdlog::level::trace);


    m_CommunicationServer = new RW::COM::CommunicatonServer(false, COM::CommunicatonServer::TypeofServer::ServiceTest, "Server", 1234, m_logger, nullptr);
    m_CommunicationServer->Register(this);
    m_CommunicationServer->Listen();
    
    m_NotiferTest = new QPushButton(this);
    m_NotiferTest->setText("Notify");
    connect(m_NotiferTest, &QPushButton::clicked, this, &ServerTestUI::OnNotify);
    m_logger->debug("rtest");
    m_logger->flush();
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
    parameter.append(Message);
    m.SetParameterList(parameter);
    m.SetIsExternal(true);
    emit NewMessage(m);
}

void ServerTestUI::OnProcessMessage(RW::COM::Message Msg)
{

}