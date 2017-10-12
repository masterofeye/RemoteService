#include "inactivitywatcher.h"
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
			m_logger(spdlog:: get("remoteservice")),
            m_TimerLogout(new QTimer(this)),
            m_ConfigManager(ConfigManager)
		{
            WorkstationKind workstationType = WorkstationKind::NON;
            QVariant type;

            m_ConfigManager->GetConfigValue(ConfigurationName::WorkstationType, type);
            workstationType = type.value<RW::WorkstationKind>();

            /*Abfrage der Zeit, wann ein User spätestens ausgelogt werden soll*/
            QVariant logoutTimeStart, logoutTimeEnd;
            m_ConfigManager->GetConfigValue(ConfigurationName::LogoutTimeStart, logoutTimeStart);
            m_ConfigManager->GetConfigValue(ConfigurationName::LogoutTimeEnd, logoutTimeEnd);
            m_LogoutTimeStart = logoutTimeStart.toTime();
            m_LogoutTimeEnd = logoutTimeEnd.toTime();

            if (workstationType == WorkstationKind::RemoteWorkstation)
            {
                QVariant timeout;
                m_ConfigManager->GetConfigValue(ConfigurationName::RwLogOutTimer, timeout);
                m_Timeout = timeout.toInt();

                m_logger->info("The current logout timeout is {}", (int)spdlog::sinks::FilterType::InactivityWatcher, m_Timeout);
            }
            else if (workstationType == WorkstationKind::BackendPC)
            {
                QVariant timeout;
                m_ConfigManager->GetConfigValue(ConfigurationName::BeLogOutTimer, timeout);
                m_Timeout = timeout.toInt();
                m_logger->info("The current logout timeout is {}", (int)spdlog::sinks::FilterType::InactivityWatcher, m_Timeout);
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
                        m_logger->info("Inactivitity Timer started.", (int)spdlog::sinks::FilterType::InactivityWatcher);
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
                        m_logger->critical("Error occoured durring Logout. Shutdown will be stopped now. ErrorID: {}", (int)spdlog::sinks::FilterType::InactivityWatcher, Msg.Result().toString().toStdString());
                        COM::Message msg;
                        msg.SetMessageID(COM::MessageDescription::IN_StopShutdownHandler);
                        emit NewMessage(msg);
                    }
                    else
                    {
                        m_isRunning = false;
                        m_logger->info("Logged was possibly processed, the shutdown timer will be started.", (int)spdlog::sinks::FilterType::InactivityWatcher);
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
                        m_logger->info("Inactivitity Timer stopped.", (int)spdlog::sinks::FilterType::InactivityWatcher);
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
                m_logger->debug("StartInactivityObservation", (int)spdlog::sinks::FilterType::InactivityWatcher);
                COM::Message msg;
                msg.SetMessageID(COM::MessageDescription::EX_StartInactivityTimer);
                msg.setIdentifier(COM::Message::GenUUID(COM::TypeofServer::RemoteHiddenHelper).toString());
                msg.SetIsExternal(true);
                QList<QVariant> paramList;
                paramList.append(m_Timeout);

				/*Wir müssen den Usernamen an den RemoteHiddenhelper schicken um sicher zu sein, 
				dass die richtige Session ausgeloggt wird.*/
				QVariant username;
				m_ConfigManager->GetConfigValue(ConfigurationName::UserName, username);
				paramList.append(username.toString());

                paramList.append(m_LogoutTimeStart);
                paramList.append(m_LogoutTimeEnd);
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
            QVariant val;
            m_ConfigManager->GetConfigValue(ConfigurationName::WorkstationState, val);
            WorkstationState state = val.value<RW::WorkstationState>();
            //Prüfen, ob der User sich schon ausgelogt hat. 
            //Wenn ja, dann nur eine Warnung absetzen und den Status auf stop(false) setzen,
            //sonst ist dies als ein Fehler auszusehen.
            if (state == WorkstationState::FREE)
            {
                m_logger->warn("Couldn't stop inactivity timer but User already logout: {}", (int)spdlog::sinks::FilterType::InactivityWatcher, "Timeout");
                m_isRunning = false;
            }
            else
            {
                m_logger->critical("Couldn't stop inactivity timer: {}", (int)spdlog::sinks::FilterType::InactivityWatcher, "Timeout");
            }
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
            msg.setIdentifier(COM::Message::GenUUID(COM::TypeofServer::RemoteHiddenHelper).toString());
            msg.SetIsExternal(true); 
            emit NewMessage(msg);
        }
	}
}