#include "WtsSessionManager.h"
#include "WtsSession.h"
#include "SessionStateMachine.h"

//TODO hier wird nur die Types benötigt
#include "RemoteDataConnectLibrary.h"
#include <spdlog\spdlog.h>

#include <windows.h>
#include <Wtsapi32.h>
#include <userenv.h>
#include <lm.h>

#pragma comment(lib, "wtsapi32.lib")

namespace RW
{
	namespace CORE
	{
		WtsSessionManager::WtsSessionManager(ConfigurationManager* Configmanager, QObject* Parent) : QObject(Parent),
			m_Logger(spdlog::get("remoteservice")),
			m_StateMachine(new SessionStateMachine(Configmanager))
		{
		}

		bool WtsSessionManager::GetCurrentlyLoggedUser(QString &Username)
		{
			LPWKSTA_USER_INFO_0 pBuf = NULL;
			LPWKSTA_USER_INFO_0 pTmpBuf;
			DWORD dwLevel = 0;
			DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
			DWORD dwEntriesRead = 0;
			DWORD dwTotalEntries = 0;
			DWORD dwResumeHandle = 0;
			DWORD i;
			DWORD dwTotalCount = 0;
			NET_API_STATUS nStatus;
			LPWSTR pszServerName = NULL;

			do // begin do
			{
				nStatus = NetWkstaUserEnum(pszServerName,
					dwLevel,
					(LPBYTE*)&pBuf,
					dwPrefMaxLen,
					&dwEntriesRead,
					&dwTotalEntries,
					&dwResumeHandle);
				if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
				{
					if ((pTmpBuf = pBuf) != NULL)
					{
						//
						// Loop through the entries.
						//
						for (i = 0; (i < dwEntriesRead); i++)
						{
							assert(pTmpBuf != NULL);

							if (pTmpBuf == NULL)
							{
								m_Logger->error("An access violation has occurred");
								break;
							}

							m_Logger->trace(QString::fromStdWString(pTmpBuf->wkui0_username).toStdString());
							QString user = QString::fromStdWString(pTmpBuf->wkui0_username);
							if (!user.contains("$"))
								Username = user;

							pTmpBuf++;
							dwTotalCount++;
						}
					}
				}
				else
				{
					m_Logger->error("A system error has occurred : {0:d}", nStatus);
				}

				// Free the allocated memory.
				if (pBuf != NULL)
				{
					NetApiBufferFree(pBuf);
					pBuf = NULL;
				}
			} while (nStatus == ERROR_MORE_DATA); // end do
			//
			// Check again for allocated memory.
			//
			if (pBuf != NULL)
				NetApiBufferFree(pBuf);
			m_Logger->trace("Total of {0:d} entries enumerated", dwTotalCount);

			if (Username.isEmpty())
				return false;

			return true;
		}




		WtsSessionManager::~WtsSessionManager()
		{
		}

		void WtsSessionManager::OnProcessMessage(COM::Message Msg)
		{	
			QString username = "";

			switch (Msg.MessageID())
			{
			case COM::MessageDescription::IN_SESSIONSTARTSERVER:
			{
				UpdateSessionsState();
				GetCurrentlyLoggedUser(username);
				SessionData* data = new SessionData(username);
                m_StateMachine->NewSession(data);
				break;
			}
			case COM::MessageDescription::IN_SESSIONLOGON:
			{
				UpdateSessionsState();
				GetCurrentlyLoggedUser(username);
				SessionData* data = new SessionData(username);
				m_StateMachine->NewSession(data);
				break;
			}
			case COM::MessageDescription::IN_SESSIONLOGOFF:
			{
				UpdateSessionsState();
				GetCurrentlyLoggedUser(username);
				SessionData* data = new SessionData(username);
				m_StateMachine->LogOff(data);
				break;
			}
			case COM::MessageDescription::IN_SESSIONCONNECT:
			{
				UpdateSessionsState();
				GetCurrentlyLoggedUser(username);
				SessionData* data = new SessionData(username);
                m_StateMachine->NewRemoteSession(data);
				break;
			}
			case COM::MessageDescription::IN_SESSIONDISCONNECT:
			{
				UpdateSessionsState();
				GetCurrentlyLoggedUser(username);
				SessionData* data = new SessionData(username);
				m_StateMachine->Disconnect(data);
				break;
			}
			case COM::MessageDescription::IN_SESSIONSHUTDOWN:
			{
				UpdateSessionsState();
				m_StateMachine->Shutdown();
				break;
			}
			default:
				break;
			}
			
		}

		void WtsSessionManager::GetSessions(std::vector<WtsSession> &Sessions)
		{
			PWTS_SESSION_INFO pSessionInfo(NULL);
			DWORD count(0);
			if (!WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &count))
			{
				DWORD err = GetLastError();
				if (err == RPC_S_INVALID_BINDING)
					return;
				else
				{
					//Error
				}
			}
			for (DWORD i = 0; i < count; ++i)
			{
				int sid = pSessionInfo[i].SessionId;
				//m_Logger->info("SessionID: {}, SessionState {}", pSessionInfo[i].SessionId, pSessionInfo[i].State);

				Sessions.push_back(WtsSession(sid, this));
			}
		}


		void WtsSessionManager::GetActiveSessions(SessionsSet & setSessions)
		{
			std::vector<WtsSession> vecSessions;
			GetSessions(vecSessions);
			for (size_t i = 0; i < vecSessions.size(); ++i)
			{
				if (vecSessions.at(i).IsActive())
				{
					std::pair<std::set<WtsSession>::iterator, bool> res = setSessions.insert(vecSessions.at(i));
					if (!res.second)
					{
						m_Logger->error("Insert active session failed", (int)spdlog::sinks::FilterType::WtsSessionMananger);
						return;
					}
				}
			}
		}

		void WtsSessionManager::GotSessionsSetGhanges(SessionsSet & addedSessions, SessionsSet & lostSessions)
		{
			GetActiveSessions(addedSessions);
			for (SessionsSet::iterator sessionIt = m_SessionsPool.begin(); sessionIt != m_SessionsPool.end(); ++sessionIt)
			{
				SessionsSet::iterator currentSessionsIt = addedSessions.find(*sessionIt);
				if (currentSessionsIt == addedSessions.end())
				{
					std::pair<SessionsSet::iterator, bool> res = lostSessions.insert(*sessionIt);
					if (!res.second)
					{
						m_Logger->error("WtsSessionManager::GotSessionsSetGhanges error: Cannot insert logged off session into pool", (int)spdlog::sinks::FilterType::WtsSessionMananger);
						return;
					}
				}
				else
				{
					addedSessions.erase(*sessionIt);
				}
			}

			// add new sessions to session pool    
			for (SessionsSet::iterator it = addedSessions.begin(); it != addedSessions.end(); ++it)
			{
				std::pair<SessionsSet::iterator, bool> res = m_SessionsPool.insert(*it);
				if (!res.second)
				{
					m_Logger->error("WtsSessionManager::GotSessionsSetGhanges error: Cannot insert logged off session into pool", (int)spdlog::sinks::FilterType::WtsSessionMananger);
					return;
				}
			}

			// erase lost sessions from session pool
			for (SessionsSet::iterator it = lostSessions.begin(); it != lostSessions.end(); ++it)
			{
				m_SessionsPool.erase(*it);
			}
		}

		void WtsSessionManager::UpdateSessionsState()
		{
			//QString currentlyLoggedUser = "";
			//bool res = false;
			//res = GetCurrentlyLoggedUser(currentlyLoggedUser);
			///*Dieser Fall sollte nur dann eintreten wenn der Service gerade gestartet wurde und noch niemand sich angemeldet hat*/
			//if (!res)
			//{
			//	m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, "Free");
			//	m_ConfigManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::FREE));
			//	m_Logger->debug("RemoteService State is: {}", "FREE");
			//}

			SessionsSet addedSessionsPool;
			SessionsSet lostSessionPool;
			GotSessionsSetGhanges(addedSessionsPool, lostSessionPool);

			//UpdateConfigurationState();
		}

		bool WtsSessionManager::GetSessionNumberByUserName(QString Username, quint16 &SessionNumber)
		{
			PWTS_SESSION_INFO sessionsBuffer = NULL;
			DWORD sessionCount = 0;
			WTS_SESSION_INFO  wts;
			m_Logger = spdlog::get("file_logger");

			if (WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &sessionsBuffer, &sessionCount))
			{
				//Loop through all Sessions
				for (quint8 i = 0; i < sessionCount; i++)
				{
					wts = sessionsBuffer[i];
					DWORD bufferSize = 0;
					LPWSTR buffer;
					if (WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, wts.SessionId, WTS_INFO_CLASS::WTSUserName, &buffer, &bufferSize))
					{
						QString username = QString::fromWCharArray(buffer, bufferSize);

						if (Username.isEmpty())
							m_Logger->warn("Username is empty.");
						int res = 0;
						QString var1 = username.trimmed().toUtf8().toUpper();
						QString var2 = Username.trimmed().toUtf8().toUpper();


						if ((res = QString::compare(var1, var2, Qt::CaseInsensitive)) == 0)
						{
							SessionNumber = wts.SessionId;
						}
						WTSFreeMemory(buffer);
					}
				}
				WTSFreeMemory(sessionsBuffer);
				return true;
			}
			else
			{
				DWORD err = GetLastError();
				m_Logger->error("WTSEnumerateSessions failed. GetLastError: {}", err);
				return false;
			}
		}
    }
}