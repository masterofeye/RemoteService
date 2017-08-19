#pragma once
#include "qobject.h"
#include <set>
#include <memory>
#include "RemoteCommunicationLibrary.h"

namespace spdlog{
	class logger;
}

namespace RW
{


    namespace CORE
    {
		enum class SessionState
		{
			Connect,
			Disconnect,
			Logon,
			LogOff,
		};

		class WtsSession;
		class SessionStateMachine;
		class ConfigurationManager;
        class WtsSessionManager : public QObject
        {
			Q_OBJECT
		private:

			std::shared_ptr<spdlog::logger> m_Logger;

			typedef std::set<WtsSession> SessionsSet;
			SessionsSet m_SessionsPool;
			SessionStateMachine *m_StateMachine = nullptr;

		private:
			void GetSessions(std::vector<WtsSession> &Sessions);
			void GetActiveSessions(SessionsSet & Sessions);
			void GotSessionsSetGhanges(SessionsSet & addedSessions, SessionsSet & lostSessions);
			bool GetCurrentlyLoggedUser(QString &Username);
		public slots:
			void OnProcessMessage(COM::Message Msg);
		signals:
			void NewMessage(COM::Message Msg);
        public:
			WtsSessionManager(ConfigurationManager*, QObject*);
            ~WtsSessionManager();

			void UpdateSessionsState();
			/*!
			@brief Gibt die Session des WTS zurück, welche dem Usernamen zugeordnet ist.
			@param [in] Username Name des User für die Suche nach der passenden Session.
			@param [out] SessionNumber Nummer der Session die mit der Username verknüpft ist.
			*/
			bool GetSessionNumberByUserName(QString Username, quint16 &SessionNumber);
        };
    }
}
