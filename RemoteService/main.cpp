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
#include "inactivitywatcher.hpp"
#include "spdlog\spdlog.h"

//TestCode
#include "RemoteDataConnectLibrary.h"
#include "Inactivitywatcher.hpp"
#include "ShutdownHandler.hpp"
#include <qt_windows.h>
#include "CommunicationServer.h"
#include "JobScheduler.h"
#include "DeviceManager.h"
#include "RemoteBoxDevice.h"
#include "AbstractDevice.h"
#include "WinApiHelper.h"



typedef SERVICE_STATUS_HANDLE(WINAPI*PRegisterServiceCtrlHandler)(const wchar_t*,LPHANDLER_FUNCTION_EX);
static PRegisterServiceCtrlHandler pRegisterServiceCtrlHandler = 0;
typedef BOOL(WINAPI*PSetServiceStatus)(SERVICE_STATUS_HANDLE, LPSERVICE_STATUS);
static PSetServiceStatus pSetServiceStatus = 0;

class RemoteService : public QtService<QApplication>
{
private: 
	QObject *m_obj;
	std::shared_ptr<spdlog::logger> m_logger;
	RW::CORE::InactivityWatcher *m_Observer;
	RW::CORE::ShutdownHandler *m_Shutdown;
	RW::CORE::JobScheduler *m_Scheduler;
	RW::CORE::CommunicationServer *m_CommunicationServer;
	RW::HW::DeviceManager *m_DeviceMng;
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

};

RemoteService::RemoteService(int argc, char **argv)
    : QtService<QApplication>(argc, argv, "RemoteService2"),
	m_obj(new QObject()),
	m_Observer(nullptr),
	m_Shutdown(nullptr),
	m_DeviceMng(new RW::HW::DeviceManager())
{

    setServiceDescription("The RemoteWorkstation Service.");
    setServiceFlags(QtServiceBase::CanBeSuspended);

}

RemoteService::~RemoteService()
{
	if (!m_Observer) delete m_Observer;
	if (!m_Shutdown) delete m_Shutdown;
}

void RemoteService::start()
{
	Sleep(10000);
	m_logger = spdlog::get("file_logger");
	if (m_logger == nullptr)
	{
		m_logger = spdlog::create<spdlog::sinks::MySqlSink>("file_logger");
	}
	m_logger->set_pattern("[%H:%M:%S:%f] [%g] [%l] [thread %t] %v ");
#ifdef REMOTESERVICE_DEBUG
	m_logger->set_level(spdlog::level::debug);
#else
	m_logger->set_level(spdlog::level::info);
#endif 
	m_logger->set_type(1);

	m_Scheduler = new RW::CORE::JobScheduler(m_DeviceMng),
	m_CommunicationServer = new RW::CORE::CommunicationServer(m_obj);

	m_logger->debug("Device manager initialize");
	m_DeviceMng->SetLogger(m_logger);
	if(!m_DeviceMng->Init())
		m_logger->error("Device manager couldn't initialized correct");
	else
		m_logger->info("Device manager initialized correct");

	//RW::HW::RemoteBoxDevice *wrapper = qobject_cast<RW::HW::RemoteBoxDevice *>(m_DeviceMng->GetDevice(RW::HW::DeviceManager::DeviceType::RemoteBox));
	//RemoteBoxWrapper::Wrapper* w = wrapper->GetDevice();
	//bool res = w->SetRelayState(0x02);

	m_Observer = new RW::CORE::InactivityWatcher("0.1");
	m_Shutdown = new RW::CORE::ShutdownHandler(m_DeviceMng, "0.1");

	QObject::connect(m_Observer, &RW::CORE::InactivityWatcher::UserInactive, m_Shutdown, &RW::CORE::ShutdownHandler::StartShutdownTimer);
    QObject::connect(m_CommunicationServer, &RW::CORE::CommunicationServer::RemoteHiddenHelperConnected, m_Observer, &RW::CORE::InactivityWatcher::StartInactivityObservation);
	//QObject::connect(m_Scheduler, &RW::CORE::JobScheduler::SendAnswer, m_Observer, &RW::CORE::InactivityWatcher::StopInactivityObservationWithCmd);
	/*Start Oberservation for user inactivity*/

	//QObject::connect(m_CommunicationServer, &RW::CORE::CommunicationServer::Message, m_Scheduler, &RW::CORE::JobScheduler::AddNewJob);
	//QObject::connect(m_Scheduler, &RW::CORE::JobScheduler::SendAnswer, m_CommunicationServer, &RW::CORE::CommunicationServer::OnMessage);

    m_CommunicationServer->Register(m_Observer);
    m_CommunicationServer->Register(m_Shutdown);

	/*m_Scheduler->start();*/
	m_CommunicationServer->Listen(42364);

	m_logger->info("Remote Service started");
	m_logger->flush();
}

void RemoteService::stop()
{
	//try{
	//	m_DeviceMng->DeInit();
	//}
	//catch (...)
	//{

	//}
	//QtServiceBase::instance()->logMessage("Remote Service stopped");
	m_logger->info("Remote Service stopped");
	m_logger->flush();
}

void RemoteService::pause()
{
	m_logger->info("Remote Service paused");
	m_logger->flush();
}

void RemoteService::resume()
{
	m_logger->info("Remote Service resumed");
	m_logger->flush();
}

void RemoteService::processCommand(int code)
{
}

void RemoteService::SessionLock()
{
	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
		m_logger->info("Session locked by User: {}", username.toStdString());
	}
	
	m_logger->flush();
}

void RemoteService::SessionUnlock(){

	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
		m_logger->info("Session unlocked by User: {}", username.toStdString());
	}
	m_logger->flush();
}

void RemoteService::RemoteConnect()
{
	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
		m_logger->info("A new remote session starts for user: {}", username.toStdString());
	}
	m_logger->flush();
}

void RemoteService::RemoteDisconnect()
{
	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
		m_logger->info("The current remote sessions end for user: {}", username.toStdString());
	}
	m_logger->flush();
}

void RemoteService::SessionLogOn()
{
	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
		m_logger->info("A new session started for user: {}", username.toStdString());
	}
	m_logger->flush();
}

void RemoteService::SessionLogOff()
{
	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
		m_logger->info("The session ends for user: {}", username.toStdString());
	}
	m_logger->flush();
}

void RemoteService::ConsoleConnect()
{
	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
		m_logger->info("Console connected by user: {}", username.toStdString());
	}
	m_logger->flush();
}

void RemoteService::ConsoleDisconnect()
{
	RW::CORE::WinApiHelper win;
	QString username;
	if (win.ReturnCurrentUser(username))
	{
		m_logger->info("Console disconnected by user: {}", username.toStdString());
	}
	m_logger->flush();
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

