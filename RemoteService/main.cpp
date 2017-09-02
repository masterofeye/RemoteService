/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QDir>
#include <QSettings>
#include "../QtService/src/qtservice.h"
#include "inactivitywatcher.h"
#include "spdlog\spdlog.h"

//TestCode
#include "RemoteDataConnectLibrary.h"
#include "RemoteCommunicationLibrary.h"
#include "ShutdownHandler.h"
#include <qt_windows.h>
#include "JobScheduler.h"
#include "DeviceManager.h"
#include "RemoteBoxDevice.h"
#include "AbstractDevice.h"
#include "WinApiHelper.h"
#include "ProcessObserver.h"
#include "AnelHome.h"
#include "NotifierHandler.h"
#include "WtsSessionManager.h"




typedef SERVICE_STATUS_HANDLE(WINAPI*PRegisterServiceCtrlHandler)(const wchar_t*,LPHANDLER_FUNCTION_EX);
static PRegisterServiceCtrlHandler pRegisterServiceCtrlHandler = 0;
typedef BOOL(WINAPI*PSetServiceStatus)(SERVICE_STATUS_HANDLE, LPSERVICE_STATUS);
static PSetServiceStatus pSetServiceStatus = 0;

class RemoteService : public QtService<QApplication>
{
private: 
	QObject *m_obj;
	std::shared_ptr<spdlog::logger> m_logger;
    RW::CORE::InactivityWatcher *m_Observer = nullptr;
    RW::CORE::ShutdownHandler *m_Shutdown = nullptr;
    RW::CORE::JobScheduler *m_Scheduler = nullptr;
    RW::COM::CommunicatonServer *m_CommunicationServer = nullptr;
    RW::HW::DeviceManager *m_DeviceMng = nullptr;
    RW::CORE::NotifierHandler *m_NotifierHandler = nullptr;
    RW::CORE::ProcessObserver *m_ProcessObserver = nullptr;
    RW::CORE::ConfigurationManager *m_Config = nullptr;
	RW::CORE::WtsSessionManager* m_SessionManager = nullptr;
public:
	RemoteService(int argc, char **argv);
	~RemoteService();

protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);
	void SessionLock();
	void SessionUnlock();
	void RemoteConnect();
	void RemoteDisconnect();
	void SessionLogOn();
	void SessionLogOff();
	void ConsoleConnect();
	void ConsoleDisconnect();
	void Shutdown();
    void DeviceRemoveComplete(QString DeviceName);
    void DeviceArrival(QString DeviceName);
    void DeviceQueryRemove(QString DeviceName);
    void DeviceQueryRemoveFailed(QString DeviceName);
    void DeviceCustomEvent(QString DeviceName);

    void HiddenHelperCrashed(QProcess::ProcessError error);
};

RemoteService::RemoteService(int argc, char **argv)
    : QtService<QApplication>(argc, argv, "RemoteService2"),
    m_obj(new QObject()),
    m_Observer(nullptr),
    m_Shutdown(nullptr),
    m_DeviceMng(nullptr),
    m_NotifierHandler(nullptr),
    m_Config(nullptr),
	m_SessionManager(nullptr)
{

    setServiceDescription("The RemoteWorkstation Service.");
    setServiceFlags(QtServiceBase::CanBeSuspended | QtServiceBase::NeedsStopOnShutdown);

}

RemoteService::~RemoteService()
{
	delete m_obj;
	if (!m_Observer) delete m_Observer;
	if (!m_Shutdown) delete m_Shutdown;
    if (!m_Scheduler) delete m_Scheduler;
}
  
void RemoteService::start()
{
#ifdef DEBUG
	Sleep(10000);
#endif

	m_logger = spdlog::get("remoteservice");
	if (m_logger == nullptr)
	{
		m_logger = spdlog::create<spdlog::sinks::MySqlSink>("remoteservice");
	}
	m_logger->set_pattern("[%H:%M:%S:%f] [%g] [%l] [thread %t] %v ");
#ifdef REMOTESERVICE_DEBUG
	m_logger->set_level(spdlog::level::debug);
#else
	m_logger->set_level(spdlog::level::info);
#endif 
	m_logger->set_type(1);

    m_Config = new RW::CORE::ConfigurationManager(m_logger,m_obj);
    m_NotifierHandler = new RW::CORE::NotifierHandler(m_obj);
    m_DeviceMng = new RW::HW::DeviceManager(m_Config,m_obj);
	m_Scheduler = new RW::CORE::JobScheduler(m_DeviceMng);
    m_CommunicationServer = new RW::COM::CommunicatonServer(true, RW::COM::TypeofServer::RemoteService,"Server", 1234, m_logger, m_obj);
	m_SessionManager = new RW::CORE::WtsSessionManager(m_Config, m_obj);
    
    m_Config->Load();
    m_Observer = new RW::CORE::InactivityWatcher("0.1", m_Config);
    m_Shutdown = new RW::CORE::ShutdownHandler(m_DeviceMng, m_Config, "0.1");

	QObject::connect(m_Observer, &RW::CORE::InactivityWatcher::UserInactive, m_Shutdown, &RW::CORE::ShutdownHandler::StartShutdownTimer);
	//QObject::connect(m_CommunicationServer, &RW::COM::CommunicationServer::RemoteHiddenHelperConnected, m_Observer, &RW::CORE::InactivityWatcher::StartInactivityObservation);
	//QObject::connect(m_Scheduler, &RW::CORE::JobScheduler::SendAnswer, m_Observer, &RW::CORE::InactivityWatcher::StopInactivityObservationWithCmd);
	/*Start Oberservation for user inactivity*/

	QObject::connect(m_CommunicationServer, &RW::COM::CommunicatonServer::NewMessage, m_Scheduler, &RW::CORE::JobScheduler::AddNewJob);
	QObject::connect(m_Scheduler, &RW::CORE::JobScheduler::SendAnswer, m_CommunicationServer, &RW::COM::CommunicatonServer::OnProcessMessage);


	m_CommunicationServer->Register(m_Observer);
	m_CommunicationServer->Register(m_Shutdown);
	m_CommunicationServer->Register(m_NotifierHandler);
	m_CommunicationServer->Register(m_SessionManager);

	RW::COM::Message message;
	message.SetMessageID(RW::COM::MessageDescription::IN_SESSIONSTARTSERVER);
	emit m_CommunicationServer->NewMessage(message);



    QVariant workstationType;
    m_Config->GetConfigValue(RW::CORE::ConfigurationName::WorkstationType, workstationType);
    if (workstationType.value<RW::WorkstationKind>() == RW::WorkstationKind::RemoteWorkstation)
    {
        m_logger->debug("Device manager initialize");
        //m_DeviceMng->SetLogger(m_logger);
        //if (!m_DeviceMng->Init())
        //    m_logger->error("Device manager couldn't initialized correct");
        //else
        //    m_logger->info("Device manager initialized correct");
    }

    m_logger->info("Start Scheduler now!");
	m_Scheduler->start();

    m_logger->info("Start CommunicationServer now!");
    m_CommunicationServer->ListenJustLocal();

    m_logger->info("Start RemoteHiddenhelper now!");
    m_ProcessObserver = new RW::CORE::ProcessObserver(m_obj);
    m_ProcessObserver->setProgram("RemoteHiddenHelper.exe");
    m_ProcessObserver->start();
    //Solange noch kein User angemeldet ist, wird erstmal der ShutdownTimer gestartet
    m_Shutdown->StartShutdownTimer();

    m_logger->info("Remote Service started", (int) spdlog::sinks::FilterType::RemoteServiceStart);
	m_logger->flush();
}

void RemoteService::stop()
{
    m_Observer->StopInactivityObservation();
    m_Shutdown->StopShutdownTimer();

    m_ProcessObserver->kill();

    m_logger->info("Remote Service stopped", (int)spdlog::sinks::FilterType::RemoteServiceStop);
 	m_logger->flush();
}

void RemoteService::pause()
{  
    m_Observer->StopInactivityObservation();
    m_Shutdown->StopShutdownTimer();
    m_logger->info("Remote Service paused",(int)spdlog::sinks::FilterType::RemoteServicePause);
	m_logger->flush();
}

void RemoteService::resume()
{
    m_Observer->StartInactivityObservation();
    m_logger->info("Remote Service resumed", (int)spdlog::sinks::FilterType::RemoteServiceResume);
	m_logger->flush();
}

void RemoteService::processCommand(int code)
{
    Q_UNUSED(code);
}

void RemoteService::SessionLock()
{
	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
        m_logger->info("Session locked by User: {}", (int)spdlog::sinks::FilterType::RemoteServiceLock, username.toStdString());
	}
	
	m_logger->flush();
}

void RemoteService::SessionUnlock(){

	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
        m_logger->info("Session unlocked by User: {}", (int)spdlog::sinks::FilterType::RemoteServiceUnlock, username.toStdString());
	}
	m_logger->flush();
}

void RemoteService::RemoteConnect()
{
	m_logger->debug("CONNECT");
	RW::COM::Message message;
	message.SetMessageID(RW::COM::MessageDescription::IN_SESSIONCONNECT);
	emit m_CommunicationServer->NewMessage(message);
	m_Config->Load();
	m_logger->flush();
}

void RemoteService::RemoteDisconnect()
{
	m_logger->debug("DISCONNECT");
	RW::COM::Message message;
	message.SetMessageID(RW::COM::MessageDescription::IN_SESSIONDISCONNECT);
	emit m_CommunicationServer->NewMessage(message);
	m_logger->flush();
}

void RemoteService::SessionLogOn()
{
	//RW::CORE::WinApiHelper win;
	//QString username;
	//if (win.ReturnCurrentUser(username))
	//{
 //       m_logger->info("A new session started for user: {}", (int)spdlog::sinks::FilterType::RemoteServiceSessionLogon, username.toStdString());
	//}
    m_CommunicationServer->ListenJustGlobal();

    m_ProcessObserver = new RW::CORE::ProcessObserver(m_obj);
    m_ProcessObserver->setProgram("RemoteHiddenHelper.exe");
    m_ProcessObserver->start();


	m_logger->debug("LOGON");
	RW::COM::Message message;
	message.SetMessageID(RW::COM::MessageDescription::IN_SESSIONLOGON);
	emit m_CommunicationServer->NewMessage(message);
    m_Config->Load();

	m_logger->flush();
}

void RemoteService::SessionLogOff()
{
    delete m_ProcessObserver;
    m_ProcessObserver = nullptr;
    m_Observer->StopInactivityObservation();
	m_logger->debug("LOGOFF");
	RW::COM::Message message;
	message.SetMessageID(RW::COM::MessageDescription::IN_SESSIONLOGOFF);
	emit m_CommunicationServer->NewMessage(message);

	m_logger->flush();
}

void RemoteService::ConsoleConnect()
{
	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
        m_logger->info("Console connected by user: {}", (int)spdlog::sinks::FilterType::RemoteServiceConsoleConnect, username.toStdString());
	}
	m_logger->flush();
}

void RemoteService::ConsoleDisconnect()
{
	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
        m_logger->info("Console disconnected by user: {}", (int)spdlog::sinks::FilterType::RemoteServiceConsoleDisconnect, username.toStdString());
	}
	m_logger->flush();
}

void RemoteService::Shutdown()
{
	RW::COM::Message message;
	message.SetMessageID(RW::COM::MessageDescription::IN_SESSIONSHUTDOWN);
	emit m_CommunicationServer->NewMessage(message);
}


void RemoteService::DeviceRemoveComplete(QString DeviceName)
{
	m_DeviceMng->DeregisterNewDevice(DeviceName);
}

void RemoteService::DeviceArrival(QString DeviceName)
{
	m_DeviceMng->RegisterNewDevice(DeviceName);

}

void RemoteService::DeviceQueryRemove(QString DeviceName)
{

}

void RemoteService::DeviceQueryRemoveFailed(QString DeviceName)
{

}

void RemoteService::DeviceCustomEvent(QString DeviceName)
{
}

int main(int argc, char **argv)
{
#if !defined(Q_OS_WIN)
    // QtService stores service settings in SystemScope, which normally require root privileges.
    // To allow testing this example as non-root, we change the directory of the SystemScope settings file.
    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
    qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
#endif
	RemoteService service(argc, argv);
    return service.exec();
}


