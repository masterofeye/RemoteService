#include "inactivitywatcher.hpp"
#include <windows.h>
#include <QLibrary>

#include "WinApiHelper.h"
#include "AbstractCommand.h"
#include "RemoteCommunicationLibrary.h"
#include "RemoteDataConnectLibrary.h"
#include "Types.h"

#include <Wtsapi32.h>
#include <userenv.h>
#include <windows.h> 

#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "userenv.lib")

namespace RW{
	namespace CORE{

		/*
		@brief
		@param Version Specified the configuration version for the DB query.
		
		*/
        InactivityWatcher::InactivityWatcher(QString Version, ConfigurationManager* ConfigManager, QObject *parent) : QObject(parent),
			m_logger(spdlog:: get("file_logger")),
            m_TimerLogout(new QTimer(this)),
            m_ConfigManager(ConfigManager)
		{
            WorkstationKind workstationType = WorkstationKind::NON;
            QVariant type;

            m_ConfigManager->GetConfigValue(ConfigurationName::WorkstationType, type);
            workstationType = type.value<RW::WorkstationKind>();

            if (workstationType == WorkstationKind::RemoteWorkstation)
            {
                QVariant timeout;
                m_ConfigManager->GetConfigValue(ConfigurationName::RwLogOutTimer, timeout);
                m_Timeout = timeout.toInt();
                m_logger->debug("The current shutdown timeout is {}", (int)spdlog::sinks::FilterType::ShutdownHandler, m_Timeout);
            }
            else if (workstationType == WorkstationKind::BackendPC)
            {
                QVariant timeout;
                m_ConfigManager->GetConfigValue(ConfigurationName::BeLogOutTimer, timeout);
                m_Timeout = timeout.toInt();
                m_logger->debug("The current shutdown timeout is {}", (int)spdlog::sinks::FilterType::ShutdownHandler, m_Timeout);
            }
		}

        void InactivityWatcher::OnProcessMessage(COM::Message Msg)
        {
            switch (Msg.MessageID())
            {
            case COM::MessageDescription::IN_StartInactivityObserver:
            {
                if (!m_isRunning)
                    StartInactivityObservation();
            }
            break;
            case COM::MessageDescription::IN_StopInactivityObserver:
            {
                if (m_isRunning)
                    StopInactivityObservation();
            }
            break;
            case COM::MessageDescription::EX_StartInactivityTimer:
                if (Msg.IsProcessed())
                {
                    m_TimerLogout->disconnect();
                    if (Msg.Success() == false)
                    {
                        m_isRunning = false;
                        m_logger->critical("Couldn't start inactivity timer: {}", (int)spdlog::sinks::FilterType::InactivityWatcher, Msg.Result().toString().toStdString());
                    }
                    else
                    {
                        m_isRunning = true;
                        m_logger->debug("Inactivitity Timer started.", (int)spdlog::sinks::FilterType::InactivityWatcher);
                    }
                }
            break;
            case COM::MessageDescription::EX_ProcessLogout:
                if (Msg.IsProcessed())
                {
                    m_TimerLogout->disconnect();
                    if (Msg.Success() == false)
                    {
                        m_isRunning = true;
                        m_logger->critical("Couldn't start inactivity timer: {}", (int)spdlog::sinks::FilterType::InactivityWatcher, Msg.Result().toString().toStdString());
                    }
                    else
                    {
                        m_isRunning = false;
                        m_logger->debug("User was logged out successfully.", (int)spdlog::sinks::FilterType::InactivityWatcher);
                        COM::Message msg;
                        msg.SetMessageID(COM::MessageDescription::IN_StartShutdownHandler);
                        emit NewMessage(msg);
                    }
                }
            break;
            case COM::MessageDescription::EX_StopInactivityTimer:
                if (Msg.IsProcessed())
                {
                    m_TimerLogout->disconnect();
                    if (Msg.Success() == false)
                    {
                        m_isRunning = true;
                        m_logger->critical("Couldn't stop inactivity timer: {}", (int)spdlog::sinks::FilterType::InactivityWatcher, Msg.Result().toString().toStdString());
                    }
                    else
                    {
                        m_isRunning = false;
                        m_logger->debug("Inactivitity Timer stopped.", (int)spdlog::sinks::FilterType::InactivityWatcher);
                    }
                }
            break;
            case COM::MessageDescription::IN_LogoutImmediately:
                    LogOutUserImmediately();
                break;
            default:
                break;
#ifdef DEBUG
                m_logger->flush();
#endif // DEBUG

            }
        }


		void InactivityWatcher::StartInactivityObservation()
		{
            if (!m_isRunning)
            {
                m_logger->trace("StartInactivityObservation", (int)spdlog::sinks::FilterType::InactivityWatcher);
                COM::Message msg;
                msg.SetMessageID(COM::MessageDescription::EX_StartInactivityTimer);
                msg.setIdentifier(COM::Message::GenUUID(COM::TypeofServer::RemoteHiddenHelper).toString());
                msg.SetIsExternal(true);
                QList<QVariant> paramList;
                paramList.append(m_Timeout);
                msg.SetParameterList(paramList);
                connect(m_TimerLogout, &QTimer::timeout, this, &InactivityWatcher::OnTimeoutStart);
                m_TimerLogout->setSingleShot(true);
                m_TimerLogout->start(5000);
                emit NewMessage(msg);
            }
		}

		void InactivityWatcher::StopInactivityObservation()
		{
            if (m_isRunning)
            {
                COM::Message msg;
                msg.SetMessageID(COM::MessageDescription::EX_StopInactivityTimer);
                msg.SetIsExternal(true);
                QList<QVariant> paramList;
                paramList.append(m_Timeout);
                msg.SetParameterList(paramList);
                connect(m_TimerLogout, &QTimer::timeout, this, &InactivityWatcher::OnTimeoutStop);
                m_TimerLogout->setSingleShot(true);
                m_TimerLogout->start(5000);

                emit NewMessage(msg);
            }
		}

        void InactivityWatcher::OnTimeoutStart()
        {
            m_logger->critical("Couldn't start inactivity timer: {}", (int)spdlog::sinks::FilterType::InactivityWatcher, "Timeout");
        }

        void InactivityWatcher::OnTimeoutStop()
        {
            m_logger->critical("Couldn't stop inactivity timer: {}", (int)spdlog::sinks::FilterType::InactivityWatcher, "Timeout");
        }



		void InactivityWatcher::StopInactivityObservationWithCmd(AbstractCommand* Cmd)
		{
			if (Cmd->CommandID() == RW::COM::MessageDescription::EX_StopInactivityTimer && Cmd->Success())
			{
				if (m_TimerLogout != nullptr && m_TimerLogout->isActive())
				{
                    m_logger->debug("Stop the observation of the user after manuell logout over :", (int)spdlog::sinks::FilterType::InactivityWatcher);//<< Cmd->Destionation().toStdString();

					m_TimerLogout->stop();
				}
			}
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG
		}

        void InactivityWatcher::LogOutUserImmediately()
        {
            COM::Message msg;
            msg.SetMessageID(COM::MessageDescription::EX_LogoutImmediately);
            msg.SetIsExternal(true); 
            emit NewMessage(msg);
        }
	}
}