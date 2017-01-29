#include "inactivitywatcher.hpp"
#include <windows.h>
#include <QLibrary>

#include "WinApiHelper.h"
#include "AbstractCommand.h"

#if defined(DEBUG) && defined(DEBUG_WITHOUT_LOGOUT)
	#define DEFAULT_TIMEOUT 60*60
#elif defined(DEBUG)
	#define DEFAULT_TIMEOUT 60*60*1000
#else
	#define DEFAULT_TIMEOUT 1000*60*180
#endif

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
			if (true)
			{
				//Set the default timeout value
				m_Timeout = DEFAULT_TIMEOUT;
			}
			else
			{
				//m_Timeout = obj->LogoutTimout();
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
			connect(m_TimerLogout, &QTimer::timeout, this, &InactivityWatcher::LogOutUser);
			//m_TimerLogout->setSingleShot(true);
			m_TimerLogout->start(5000);
			Sleep(100);
			qApp->processEvents();
			m_logger->debug("Inactivitity Timer started.");
			
		}

		void InactivityWatcher::StopInactivityObservation()
		{
			if (m_TimerLogout != nullptr && m_TimerLogout->isActive())
			{
				m_TimerLogout->stop();
				m_logger->debug("Inactivitity Timer stopped.");
			}
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG
		}

		void InactivityWatcher::StopInactivityObservationWithCmd(AbstractCommand* Cmd)
		{
			if (Cmd->CommandID() == (quint16) RW::CORE::CommandIds::LOGOUT && Cmd->Success())
			{
				if (m_TimerLogout != nullptr && m_TimerLogout->isActive())
				{
					m_logger->debug("Stop the observation of the user after manuell logout over :");//<< Cmd->Destionation().toStdString();

					m_TimerLogout->stop();
				}
			}
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG
		}


		void InactivityWatcher::LogOutUser()
		{
			m_logger->debug("LogoutUser was called.");
			if (GetLastInputTime() >= m_Timeout)
			{
#ifdef DEBUG_WITHOUT_LOGOUT
				m_TimerLogout->stop();
				m_logger->debug("User is logged out now.");
				m_logger->flush();

				emit UserInactive();
#else
				WinApiHelper helper;
				quint64 sessionId = 0;
				if (!helper.QueryActiveSession(sessionId))
				{
					m_logger->error("Log-off of user failed.");

				}
				else
				{
					if (helper.LogOff(sessionId))
					{
						m_TimerLogout->stop();
						m_logger->info("User is logged out now.");
						m_logger->flush();
						emit UserInactive();
					}
					else
					{
						m_logger->error("Log-off of user failed.");
					}
				}
#endif // DEBUG
			}
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG
		}
	}
}