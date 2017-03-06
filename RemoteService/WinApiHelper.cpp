#include "WinApiHelper.h"

#include <Wtsapi32.h>
#include <userenv.h>
#include <windows.h> 
#include <lm.h>

#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "netapi32.lib")

namespace RW
{
	namespace CORE
	{
		WinApiHelper::WinApiHelper() :m_logger(spdlog::get("file_logger"))
		{
		}


		WinApiHelper::~WinApiHelper()
		{
		}

		/*
		*@brief Returns the session number, that is currently in state active.
		*@return True if a session was in the active state
		*/
		bool WinApiHelper::QueryActiveSession(quint64 &SessioNumber)
		{
			SessioNumber = 0;
			PWTS_SESSION_INFO  pSessionsBuffer = NULL;
			DWORD dwSessionCount = 0;
			WTS_SESSION_INFO  wts;

			if (WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionsBuffer, &dwSessionCount))
			{
				//Loop through all Sessions
				for (quint8 i = 0; i < dwSessionCount; i++)
				{
					wts = pSessionsBuffer[i];
					//Nur aktive Sessions weden berücksichtigt
					if (wts.State == WTSActive)
					{
						SessioNumber = wts.SessionId;
						m_logger->debug("SessionNumber is: ");// << SessioNumber;
					}
				}
				return true;
			}
			else
			{
				DWORD err = GetLastError();
				m_logger->error("WTSEnumerateSessions failed. GetLastError: ");// << err;
				return false;
			}
		}





		bool WinApiHelper::ReturnCurrentUser(QString &Username)
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

			std::shared_ptr<spdlog::logger> logger = spdlog::get("file_logger");
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
								//
								// Only members of the Administrators local group
								//  can successfully execute NetWkstaUserEnum
								//  locally and on a remote server.
								//
								
								logger->error("An access violation has occurred");
								break;
							}
							//
							// Print the user logged on to the workstation. 
							//
							
							logger->trace(QString::fromStdWString(pTmpBuf->wkui0_username).toStdString());
							Username = Username.fromStdWString(pTmpBuf->wkui0_username);

							pTmpBuf++;
							dwTotalCount++;
						}
					}
				}
				else
					logger->error("A system error has occurred : {0:d}", nStatus);
				//
				// Free the allocated memory.
				//
				if (pBuf != NULL)
				{
					NetApiBufferFree(pBuf);
					pBuf = NULL;
				}
			}
			while (nStatus == ERROR_MORE_DATA); // end do
			//
			// Check again for allocated memory.
			//
			if (pBuf != NULL)
				NetApiBufferFree(pBuf);
			logger->trace("Total of {0:d} entries enumerated", dwTotalCount);
			return true;
		}
	}
}