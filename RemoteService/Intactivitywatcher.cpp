#include "inactivitywatcher.hpp"
#include <windows.h>
#include <QLibrary>

#include "MongoDbClient.hpp"
#include "GeneralObject.hpp"
#include "WinApiHelper.h"
#include "AbstractCommand.h"

#define DEFAULT_TIMEOUT 60*60*1000


namespace RW{
	namespace CORE{

		/*
		@brief
		@param Version Specified the configuration version for the DB query.
		
		*/
		InactivityWatcher::InactivityWatcher(QString Version, QObject *parent) : QObject(parent),
			m_logger(spdlog::get("file_logger")),
			m_TimerLogout(nullptr)
		{
			MONGO::GeneralObject *obj = (MONGO::GeneralObject*) MONGO::MongoDbClient::Instance()->GeneralInformation(Version);
			if (obj == nullptr)
			{
				//Set the default timeout value
				m_Timeout = DEFAULT_TIMEOUT;
			}
			else
			{
				m_Timeout = obj->LogoutTimout();
			}
		}

		uint InactivityWatcher::GetLastInputTime()
		{
			LASTINPUTINFO li;
			li.cbSize = sizeof(LASTINPUTINFO);
			::GetLastInputInfo(&li);

			DWORD te = ::GetTickCount();
			float elapsed = (te - li.dwTime) / 100.0;
			return elapsed;
		}

		void InactivityWatcher::StartInactivityObservation()
		{
			if (m_TimerLogout == nullptr)
			{
				m_TimerLogout = new QTimer(this);
			}
			connect(m_TimerLogout, SIGNAL(timeout()), this, SLOT(LogOutUser));
			m_TimerLogout->start(m_Timeout);
		}

		void InactivityWatcher::StopInactivityObservation()
		{
			if (m_TimerLogout != nullptr && m_TimerLogout->isActive())
				m_TimerLogout->stop();
		}

		void InactivityWatcher::StopInactivityObservationWithCmd(AbstractCommand* Cmd)
		{
			if (Cmd->CommandID() == (quint16) RW::CORE::CommandIds::LOGOUT && Cmd->Success())
			{
				if (m_TimerLogout != nullptr && m_TimerLogout->isActive())
				{
					m_logger->debug("Stop the observation of the user after manuell logout over :") << Cmd->Destionation().toStdString();

					m_TimerLogout->stop();
				}
			}
		}


		void InactivityWatcher::LogOutUser()
		{
			if (GetLastInputTime() == m_Timeout)
			{
				WinApiHelper helper;
				quint64 sessionId = 0;
				if (!helper.QueryActiveSession(sessionId))
				{
					m_logger->error("Log-off of user failed.");
					return ;
				}

				if (helper.LogOff(sessionId))
				{
					m_logger->info("User logged out.");
					m_TimerLogout->stop();
					emit UserInactive();
				}
				else
				{
					m_logger->error("Log-off of user failed.");
				}
			}
		}
	}
}