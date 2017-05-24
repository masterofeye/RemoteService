#include "WinApiHelper.h"

#include <Wtsapi32.h>
#include <userenv.h>
#include <windows.h> 
#include <lm.h>
#include <SetupAPI.h>
#include <devpkey.h>
#include <devguid.h>
#include <QApplication.h>

#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "SetupAPI.lib")
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
                WTSFreeMemory(pSessionsBuffer);
                return true;
            }
            else
            {
                WTSFreeMemory(pSessionsBuffer);
                DWORD err = GetLastError();
                m_logger->error("WTSEnumerateSessions failed. GetLastError: ");// << err;
                return false;
            }
        }

        /**
        @brief The user need to destroy the returned handle with CloseHandle.
        **/
        HANDLE WinApiHelper::GetCurrentUserToken()
        {
            HANDLE  currentToken = 0;
            HANDLE primaryToken = 0;
            quint64 SessioNumber = 0;

            if (!QueryActiveSession(SessioNumber))
                return 0;

            BOOL bRet = WTSQueryUserToken(SessioNumber,&currentToken);
            if (!bRet)
            {
                long nError = GetLastError();
                m_logger->error("Couldn't query user token: {}", nError);
                return 0;
            }

            bRet = DuplicateTokenEx(currentToken, TOKEN_ASSIGN_PRIMARY | TOKEN_ALL_ACCESS, 0, SecurityImpersonation, TokenPrimary, &primaryToken);
            if (!bRet)
            {
                long nError = GetLastError();
                m_logger->error("Couldn't duplicate user token: {}", nError);
                return 0;
            }
            return primaryToken;
        }

        bool WinApiHelper::CreateProcessAsCurrentUser(QString Programm, QString Arguments)
        {
            QString programmDir = QApplication::applicationDirPath();
            Programm = programmDir + "\\" + Programm;
            m_logger->debug("Process path is " + Programm.toStdString());

            HANDLE primaryToken = GetCurrentUserToken();
            if (primaryToken == 0)
            {
                return FALSE;
            }

            STARTUPINFO StartupInfo;
            PROCESS_INFORMATION processInfo;
            StartupInfo.cb = sizeof(STARTUPINFO);
            SECURITY_ATTRIBUTES Security1;
            SECURITY_ATTRIBUTES Security2;

            ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));
            ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));

            void* lpEnvironment = NULL;
            BOOL res = CreateEnvironmentBlock(&lpEnvironment,primaryToken, FALSE);

            QString desctop = "winsta0\\default";
            LPWSTR lpdesctop = (LPWSTR)desctop.utf16();
            StartupInfo.lpDesktop = lpdesctop;
            StartupInfo.cb = sizeof(StartupInfo);

            if (!res)
            {
                long nError = GetLastError();
                m_logger->error("Couldn't query enviroment from current user: {}", nError);
                CloseHandle(primaryToken);
                return false;
            }

            LPWSTR path = (LPWSTR)Programm.utf16();

            BOOL result = CreateProcessAsUser(primaryToken,path,0,0,0,false,
                NORMAL_PRIORITY_CLASS |
                CREATE_UNICODE_ENVIRONMENT,lpEnvironment,0,
                &StartupInfo, &processInfo);
            if (!result)
            {
                long nError = GetLastError();
                m_logger->error("Couldn't start process as current user: {}", nError);
                DestroyEnvironmentBlock(lpEnvironment);
                CloseHandle(primaryToken);
                return false;
            }
            DWORD theExitCode = 0;
            res = GetExitCodeProcess(processInfo.hProcess, &theExitCode);
            if (!res)
            {
                long nError = GetLastError();
                m_logger->error("Couldn't query enviroment from current user: {}", nError);
                CloseHandle(primaryToken);
                return false;
            }

            DestroyEnvironmentBlock(lpEnvironment);
            CloseHandle(primaryToken);
            return true;
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
							QString user = QString::fromStdWString(pTmpBuf->wkui0_username);
                            if (!user.contains("$"))
                                Username = user;

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

        bool WinApiHelper::QueryActiveHW()
        {
            HDEVINFO DeviceInfoSet;
            SP_DEVINFO_DATA DeviceInfoData;
			DEVPROPTYPE devicePropertyType;
			DWORD error, dwPropertyRegDataType, dwSize;
			TCHAR szDesc[4096], szHardwareIDs[4096];
			GUID *guidDev = (GUID*)&GUID_DEVCLASS_USB;
            DeviceInfoSet = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);

            if (DeviceInfoSet == INVALID_HANDLE_VALUE)
            {
                m_logger->error("Unable to create device information set {}", GetLastError());
                return false;
            }

            ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
            DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
            quint16 DeviceIndex = 0;

            while (SetupDiEnumDeviceInfo(DeviceInfoSet, DeviceIndex, &DeviceInfoData))
			{
				DeviceIndex++;

                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_DEVICEDESC,
					&dwPropertyRegDataType, (BYTE*)szDesc,
					sizeof(szDesc),   // The size, in bytes
					&dwSize))
				{

					QString testsssss = QString::fromWCharArray(szDesc);
					m_logger->debug(testsssss.toStdString());
				}

				if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID,
					&dwPropertyRegDataType, (BYTE*)szHardwareIDs,
					sizeof(szHardwareIDs),    // The size, in bytes
					&dwSize)) 
					m_logger->debug(QString::fromWCharArray(szHardwareIDs).toStdString());


                /*if (!SetupDiGetDeviceProperty( DeviceInfoSet, &DeviceInfoData, &DEVPKEY_Device_Class, &devicePropertyType,
                    (PBYTE)&DevGuid,
                    sizeof(GUID),
                    &Size,
					0) || devicePropertyType != DEVPROP_TYPE_GUID) {

                    error = GetLastError();

					switch (error)
					{
					case ERROR_INVALID_FLAGS:
						m_logger->error("The value of Flags is not zero.");
						break;
					case ERROR_INVALID_HANDLE:
						m_logger->error("The device information set that is specified by DevInfoSet is not valid.");
						break;
					case ERROR_INVALID_PARAMETER:
						m_logger->error("A supplied parameter is not valid.One possibility is that the device information element is not valid.");
						break;
					case ERROR_INVALID_REG_PROPERTY:
						m_logger->error("The property key that is supplied by PropertyKey is not valid.");
						break;
					case ERROR_INVALID_DATA:
						m_logger->error("An unspecified internal data value was not valid.");
						break;
					case ERROR_INVALID_USER_BUFFER:
						m_logger->error("A user buffer is not valid. One possibility is that PropertyBuffer is NULL and PropertBufferSize is not zero.");
						break;
					case ERROR_NO_SUCH_DEVINST:
						m_logger->error("The device instance that is specified by DevInfoData does not exist.");
						break;
					case ERROR_INSUFFICIENT_BUFFER:
						m_logger->error("The PropertyBuffer buffer is too small to hold the requested property value, or an internal data buffer that was passed to a system call was too small.");
						break;
					case ERROR_NOT_ENOUGH_MEMORY:
						m_logger->error("There was not enough system memory available to complete the operation.");
						break;
					case ERROR_NOT_FOUND:
						m_logger->error("The requested device property does not exist.");
						break;
					case ERROR_ACCESS_DENIED:
						m_logger->error("The caller does not have Administrator privileges.");
						break;
					default:
						break;
					}


                    if (Error == ERROR_NOT_FOUND) {
                    }
                }*/
            }

            if (DeviceInfoSet) {
                SetupDiDestroyDeviceInfoList(DeviceInfoSet);
            }
        }


        bool WinApiHelper::Shutdown()
        {
            bool res = InitiateSystemShutdownEx(NULL, NULL, 0, TRUE, FALSE, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
            if (!res)
            {
                DWORD error = GetLastError();
                m_logger->critical("Shutdown incomplete: {}", error);
                
            }
            return res;
        }
	}
}