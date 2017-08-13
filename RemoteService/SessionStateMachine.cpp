#include "SessionStateMachine.h"
#include "RemoteDataConnectLibrary.h"
#include <spdlog\spdlog.h>
#include <qmetaobject.h>

namespace RW{
	namespace CORE{
		SessionStateMachine::SessionStateMachine(ConfigurationManager* Configmanager) : StateMachine(ST_MAX_STATES),
			m_Logger(spdlog::get("remoteservice")),
			m_ConfigManager(Configmanager)
		{
		}

		SessionStateMachine::~SessionStateMachine()
		{    

		}

		void SessionStateMachine::NewSession(SessionData* Data = nullptr)
		{
			BEGIN_TRANSITION_MAP						        // - Current State -
				TRANSITION_MAP_ENTRY(ST_STARTSERVICE)			    // ST_STARTSERVICE
				TRANSITION_MAP_ENTRY(ST_LOGON)				    // ST_CONNECT
				TRANSITION_MAP_ENTRY(ST_CONNECT)				// ST_DISCONNECT
				TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)				// ST_LOGON
				TRANSITION_MAP_ENTRY(ST_CONNECT)				// ST_LOGOFF
				TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)				// ST_SHUTDOWN
			END_TRANSITION_MAP(Data)
		}

		void SessionStateMachine::Disconnect(SessionData* Data = nullptr)
		{
			BEGIN_TRANSITION_MAP						        // - Current State -
				TRANSITION_MAP_ENTRY(ST_DISCONNECT)			    // ST_STARTSERVICE
				TRANSITION_MAP_ENTRY(ST_DISCONNECT)				// ST_CONNECT
				TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)				// ST_DISCONNECT
				TRANSITION_MAP_ENTRY(ST_DISCONNECT)				// ST_LOGON
				TRANSITION_MAP_ENTRY(ST_LOGOFF)				    // ST_LOGOFF
				TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)				// ST_SHUTDOWN
			END_TRANSITION_MAP(Data)
		}

		void SessionStateMachine::LogOff(SessionData* Data = nullptr)
		{
			BEGIN_TRANSITION_MAP						        // - Current State -
				TRANSITION_MAP_ENTRY(ST_LOGOFF)			        // ST_STARTSERVICE
				TRANSITION_MAP_ENTRY(ST_LOGOFF)				    // ST_CONNECT
				TRANSITION_MAP_ENTRY(ST_LOGOFF)				    // ST_DISCONNECT
				TRANSITION_MAP_ENTRY(ST_LOGOFF)				    // ST_LOGON
				TRANSITION_MAP_ENTRY(ST_SHUTDOWN)				// ST_LOGOFF
				TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)				// ST_SHUTDOWN
			END_TRANSITION_MAP(Data)
		}




		// state machine sits here when motor is not running
		void SessionStateMachine::ST_StartService(SessionData* Data)
		{
			if (Data->username.isEmpty())
			{
				m_Logger->info("ST_STARTSERVICE");
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, "Free");
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::FREE));
				m_Logger->debug("RemoteService State is: {}", "FREE");
			}
			else
			{
				m_Logger->info("ST_STARTSERVICE");
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, Data->username);
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::ON));
				m_Logger->debug("RemoteService State is: {}", "ON");
			}

		}

		void SessionStateMachine::ST_Connect(SessionData* Data)
		{
			if (!Data->username.isEmpty())
			{
				m_Logger->info("ST_CONNECT");
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, Data->username);
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::OCCUPY));
				m_Logger->debug("RemoteService State is: {}", "OCCUPY");
			}
			else
			{
				m_Logger->info("ST_CONNECT");
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, "Free");
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::FREE));
				m_Logger->debug("RemoteService State is: {}", "FREE");
			}

		}


		void SessionStateMachine::ST_Disconnect(EventData* Data)
		{

			m_Logger->info("ST_DISCONNECT");
			m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, "Occupy");
			m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::OCCUPY));
			m_Logger->debug("RemoteService State is: {}", "OCCUPY");

		}

		void SessionStateMachine::ST_Logon(SessionData* Data)
		{
			if (!Data->username.isEmpty())
			{
				m_Logger->info("ST_LOGON");
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, Data->username);
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::ON));
				m_Logger->debug("RemoteService State is: {}", "ON");
			}
			else
			{
				m_Logger->info("ST_LOGON");
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, "Defect");
				m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::DEFECT));
				m_Logger->debug("RemoteService State is: {}", "DEFECT");
			}

		}

		void SessionStateMachine::ST_LogOff(EventData* Data)
		{
			m_Logger->info("ST_LOGOFF");
			m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, "Free");
			m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::FREE));
			m_Logger->debug("RemoteService State is: {}", "FREE");

		}

		void SessionStateMachine::ST_Shutdown(EventData* Data)
		{
			m_Logger->info("ST_SHUTDOWN");
			m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, "Offline");
			m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::OFF));
			m_Logger->debug("RemoteService State is: {}", "OFF");

		}
	}
}
