#pragma once
#include "StateMachine.h"
#include <memory>

namespace spdlog{
	class logger;
}

namespace RW{
	namespace CORE{
		struct SessionData : public EventData
		{
			QString username = "";
			SessionData(QString Name) :username(Name)
			{}
		};

		class ConfigurationManager;
		class SessionStateMachine : public StateMachine
		{
			Q_OBJECT
		private:
			std::shared_ptr<spdlog::logger> m_Logger;
			ConfigurationManager *m_ConfigManager = nullptr;
		public:
			SessionStateMachine(ConfigurationManager* Configmanager);
			~SessionStateMachine();

			void NewSession(SessionData* Data);
			void Disconnect(SessionData* Data);
			void LogOff(SessionData* Data);


		private:
			// state machine state functions
			void ST_StartService(SessionData*);
			void ST_Connect(SessionData*);
			void ST_Disconnect(EventData*);
			void ST_Logon(SessionData*);
			void ST_LogOff(EventData*);
			void ST_Shutdown(EventData*);

			// state map to define state function order
			BEGIN_STATE_MAP
				STATE_MAP_ENTRY(&SessionStateMachine::ST_StartService)
				STATE_MAP_ENTRY(&SessionStateMachine::ST_Connect)
				STATE_MAP_ENTRY(&SessionStateMachine::ST_Disconnect)
				STATE_MAP_ENTRY(&SessionStateMachine::ST_Logon)
				STATE_MAP_ENTRY(&SessionStateMachine::ST_LogOff)
				STATE_MAP_ENTRY(&SessionStateMachine::ST_Shutdown)
			END_STATE_MAP

			// state enumeration order must match the order of state
			// method entries in the state map
			enum E_States {
				ST_STARTSERVICE = 0,
				ST_CONNECT,
				ST_DISCONNECT,
				ST_LOGON,
				ST_LOGOFF,
				ST_SHUTDOWN,
				ST_MAX_STATES
			};
			Q_ENUM(E_States)
		};
	}
}
