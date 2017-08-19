#pragma once
#include "qobject.h"
#include <memory>
namespace spdlog{
	class logger;
}

namespace RW{
    namespace CORE{
        class WtsSession : public QObject
        {
			Q_OBJECT
            quint32 m_SessionID = 0;
			std::shared_ptr<spdlog::logger> m_Logger;
        public:
            WtsSession(quint16 SessionId, QObject* Parent);
            ~WtsSession();

			WtsSession::WtsSession(const WtsSession& other) 
			{
				m_SessionID = other.m_SessionID;
				m_Logger = other.m_Logger;
			}

			WtsSession& WtsSession::operator=(WtsSession& other)
			{
				m_SessionID = other.m_SessionID;
				m_Logger = other.m_Logger;
				return *this;
			}

			WtsSession::WtsSession(WtsSession&& other)
			{
				m_SessionID = other.m_SessionID;
				m_Logger = other.m_Logger;
			}

			WtsSession& WtsSession::operator=(WtsSession&& other)
			{
				m_SessionID = other.m_SessionID;
				m_Logger = other.m_Logger;
				return *this;
			}

			/*@brief Gibt die aktuelle SessionID zurück.
			@return aktuelle Session ID.
			*/
			inline quint16 GetSessionID() const { return m_SessionID; }

			/*!
			@brief Überprüft ob die Session, die durch die WtsSession Instanz verwarltet wird aktive ist.
			@return True, wenn die Session aktive ist, sonst false.
			*/
			bool IsActive() const;

			bool IsDisconnected() const;
			/*!
			@brief Gibt die .
			@return True, wenn die Session aktive ist, sonst false.
			*/
			double WtsSession::GetLogonTime() const;

			/*!
			@brief Gibt den aktuellen Usernamen für die "aktive" Session zurück
			@return Username der aktuellen Session.
			*/
			QString GetSessionUserName() const;

			/*!
			@brief Gibt den aktuellen Domain-Namen für die "aktive" Session zurück
			@return Domain-Namen der aktuellen Session.
			*/
			QString GetDomainName() const;

			bool operator < (const WtsSession & other) const
			{
				return m_SessionID < other.GetSessionID();
			}
        };

    }
}