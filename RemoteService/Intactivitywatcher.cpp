#include "inactivitywatcher.hpp"
#include <windows.h>
#include <QLibrary>

#include "WinApiHelper.h"
#include "AbstractCommand.h"
#include "RemoteCommunicationLibrary.h"



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


		void InactivityWatcher::StartInactivityObservation()
		{
			COM::Message msg;
			msg.SetMessageID(COM::MessageDescription::EX_StartInactivityTimer);
			msg.SetIsExternal(true);
			QList<QVariant> paramList;
			paramList.append(m_Timeout);
			msg.SetParameterList(paramList);

			emit NewMessage(msg);
			m_logger->debug("Inactivitity Timer started.");
#ifdef DEBUG
            m_logger->flush();
#endif // DEBUG
		}

		void InactivityWatcher::StopInactivityObservation()
		{
			COM::Message msg;
			msg.SetMessageID(COM::MessageDescription::EX_StopInactivityTimer);
			msg.SetIsExternal(true);
			QList<QVariant> paramList;
			paramList.append(m_Timeout);
			msg.SetParameterList(paramList);

			emit NewMessage(msg);
            m_logger->debug("Inactivitity Timer stopped.");
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG
		}

		void InactivityWatcher::StopInactivityObservationWithCmd(AbstractCommand* Cmd)
		{
			if (Cmd->CommandID() == RW::COM::MessageDescription::EX_StopInactivityTimer && Cmd->Success())
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

		void InactivityWatcher::OnProcessMessage(COM::Message Msg)
		{
			switch (Msg.MessageID())
			{
			case COM::MessageDescription::EX_StartInactivityTimer:
				break;
			case COM::MessageDescription::EX_StopInactivityTimer:
				break;
			default:
					break;
			}
        }
	}
}