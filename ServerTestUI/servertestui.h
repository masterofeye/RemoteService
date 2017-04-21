#ifndef SERVERTESTUI_H
#define SERVERTESTUI_H

#include <QtWidgets/QMainWindow>
#include "ui_servertestui.h"
#include <RemoteCommunicationLibrary.h>
#include <spdlog\logger.h>

using namespace RW;

class QPushButton;

class ServerTestUI : public QMainWindow
{
    Q_OBJECT
private:
    RW::COM::CommunicatonServer* m_CommunicationServer;
    QPushButton* m_NotiferTest = nullptr;
    std::shared_ptr<spdlog::logger> m_logger;
public:
    ServerTestUI(QWidget *parent = 0);
    ~ServerTestUI();

private:
    Ui::ServerTestUIClass ui;

public slots:
    void OnNotify();
    void OnProcessMessage(COM::Message Msg);
signals:
    void NewMessage(COM::Message Msg);
};

#endif // SERVERTESTUI_H
