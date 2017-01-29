#include "WinApiHelper.h"

#include <Wtsapi32.h>
#include <userenv.h>

#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "userenv.lib")

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

		/*
		@brief Logs off the current user from the active session. It is irelevant, if it is a console or rdp session.
		It needs the session number, which can be queried with the QueryActiveSession method.
		@param SessioNumber The session number to the current active session.
		@return
		*/
		bool WinApiHelper::LogOff(quint64 SessioNumber)
		{
			if (!WTSLogoffSession(WTS_CURRENT_SERVER_HANDLE, SessioNumber, true))
			{
				DWORD err = GetLastError();
				m_logger->error("WTSLogoffSession failed. GetLastError: ");// << err;
				return false;
			}
			else
			{
				//This will be logged by the service. No logging needed here.
				return true;
			}
		}

		/*
		@brief Shutdown the RemoteWorkstation.
		@note This funktion has no influence of the connected hardware.
		@return
		*/
		bool WinApiHelper::Shutdown()
		{
			HANDLE hToken = NULL;
			TOKEN_PRIVILEGES tkp = { 0 };
			bool bRet = false;

			// Get a token for this process. 
			if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
				if (LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
					tkp.PrivilegeCount = 1;  // one privilege to set    
					tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

					// Get the shutdown privilege for this process. 
					if (AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0)) {
						::CloseHandle(hToken);

						if (ERROR_SUCCESS == GetLastError()) {

							DWORD dwFlags = EWX_POWEROFF;
							DWORD dwReason = SHTDN_REASON_MAJOR_SYSTEM;

							if (ExitWindowsEx(dwFlags, dwReason)) {
								bRet = true;
							}
						}
					}
				}
			}

			return bRet;
		}
	}
}