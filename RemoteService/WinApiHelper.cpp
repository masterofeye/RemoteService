#include "WinApiHelper.h"

#include <Wtsapi32.h>
#include <userenv.h>
#include <windows.h> 
#include <lm.h>
#include <SetupAPI.h>
#include <devpkey.h>
#include <devguid.h>
#include <QApplication.h>
#include <QAbstractEventDispatcher>
#include <QAbstractNativeEventFilter>

#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "SetupAPI.lib")
#pragma comment(lib, "netapi32.lib")




namespace RW
{
    namespace CORE
    {
        class ChangeDeviceFilter : public QAbstractNativeEventFilter
        {
        public:
            ChangeDeviceFilter() {}
            bool nativeEventFilter(const QByteArray &eventType, void *message, long *result)
            {
                MSG *msg;
                static int i = 0;

                msg = (MSG*)message;

                if (msg->message == WM_DEVICECHANGE)
                {
                    i++;
                    //qDebug() << "Count:" << i << "wParam:" << msg->wParam << "lParam:" << msg->lParam;
                }
                return true;
            }
        };

        Q_GLOBAL_STATIC(ChangeDeviceFilter, changeDeviceFilter)



        WinApiHelper::WinApiHelper() :m_logger(spdlog::get("remoteservice"))
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

			std::shared_ptr<spdlog::logger> logger = spdlog::get("remoteservice");
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

            if (Username.isEmpty())
                return false;

			return true;
		}

        bool WinApiHelper::QueryActiveHW()
        {
            HDEVINFO DeviceInfoSet;
            SP_DEVINFO_DATA DeviceInfoData;
			DEVPROPTYPE devicePropertyType;
			DWORD error, dwPropertyRegDataType, dwSize;
            TCHAR szDesc[4096], szHardwareIDs[4096], szFriendlyName[4096], szLocalInform[4096];
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
                    m_logger->debug("Description: {}",testsssss.toStdString());
                }

                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID,
                    &dwPropertyRegDataType, (BYTE*)szHardwareIDs,
                    sizeof(szHardwareIDs),    // The size, in bytes
                    &dwSize))
                    m_logger->debug("HW-ID: {}", QString::fromWCharArray(szHardwareIDs).toStdString());

                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME,
                    &dwPropertyRegDataType, (BYTE*)szFriendlyName,
                    sizeof(szFriendlyName),    // The size, in bytes
                    &dwSize))
                    m_logger->debug("FriendlyName: {}", QString::fromWCharArray(szFriendlyName).toStdString());

                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_LOCATION_INFORMATION,
                    &dwPropertyRegDataType, (BYTE*)szLocalInform,
                    sizeof(szLocalInform),    // The size, in bytes
                    &dwSize))
                    m_logger->debug("Location: {}", QString::fromWCharArray(szLocalInform).toStdString());

            }

            if (DeviceInfoSet) {
                SetupDiDestroyDeviceInfoList(DeviceInfoSet);
            }

            QAbstractEventDispatcher::instance()->installNativeEventFilter(changeDeviceFilter());

        }





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