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
#include <qvector.h>

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
                Q_UNUSED(result)
                    Q_UNUSED(eventType)
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
                        m_logger->trace("SessionNumber is: {}", SessioNumber);// << SessioNumber;
                    }
                }
                WTSFreeMemory(pSessionsBuffer);
                return true;
            }
            else
            {
                WTSFreeMemory(pSessionsBuffer);
                DWORD err = GetLastError();
                m_logger->error("WTSEnumerateSessions failed. GetLastError: {}", err);
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

            BOOL bRet = WTSQueryUserToken(SessioNumber, &currentToken);
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


            ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));
            ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));

            void* lpEnvironment = NULL;
            BOOL res = CreateEnvironmentBlock(&lpEnvironment, primaryToken, FALSE);

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

            BOOL result = CreateProcessAsUser(primaryToken, path, 0, 0, 0, false,
                NORMAL_PRIORITY_CLASS |
                CREATE_UNICODE_ENVIRONMENT, lpEnvironment, 0,
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
            } while (nStatus == ERROR_MORE_DATA); // end do
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

        bool WinApiHelper::QueryActiveHW(QVector<DeviceInformation>  &Info)
        {
            HDEVINFO DeviceInfoSet;
            SP_DEVINFO_DATA DeviceInfoData;
            //DEVPROPTYPE devicePropertyType;
            DWORD dwSize = 0, dwPropertyRegDataType = 0;
            TCHAR szDesc[4096], szHardwareIDs[4096], szFriendlyName[4096], szLocalInform[4096],
                szClassGuid[4096], szClass[4096], szAddress[4096], szBusGuid[4096], szDevType[4096],
                szEnumeratorName[4096], szDeviceManufacturer[4096], szPhysicalDeviceObjectName[4096], szServiceName[4096];



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
                if (DeviceIndex > Info.count() - 10)
                    Info.resize(DeviceIndex + 20);

                DeviceIndex++;

                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_DEVICEDESC,
                    &dwPropertyRegDataType, (BYTE*)szDesc,
                    sizeof(szDesc),   // The size, in bytes
                    &dwSize))
                {
                    Info[DeviceIndex].DeviceDescription = QString::fromWCharArray(szDesc);
                }

                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID,
                    &dwPropertyRegDataType, (BYTE*)szHardwareIDs,
                    sizeof(szHardwareIDs),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].HardwareID = QString::fromWCharArray(szHardwareIDs);

                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME,
                    &dwPropertyRegDataType, (BYTE*)szFriendlyName,
                    sizeof(szFriendlyName),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].FiendlyName = QString::fromWCharArray(szFriendlyName);

                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_LOCATION_INFORMATION,
                    &dwPropertyRegDataType, (BYTE*)szLocalInform,
                    sizeof(szLocalInform),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].LocationInfo = QString::fromWCharArray(szLocalInform);
                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_ADDRESS,
                    &dwPropertyRegDataType, (BYTE*)szAddress,
                    sizeof(szAddress),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].Address = QString::fromWCharArray(szAddress);
                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_CLASS,
                    &dwPropertyRegDataType, (BYTE*)szClass,
                    sizeof(szClass),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].Class = QString::fromWCharArray(szClass);
                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_BUSTYPEGUID,
                    &dwPropertyRegDataType, (BYTE*)szBusGuid,
                    sizeof(szBusGuid),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].BusTypeGuiD = QString::fromWCharArray(szBusGuid);
                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_CLASSGUID,
                    &dwPropertyRegDataType, (BYTE*)szClassGuid,
                    sizeof(szClassGuid),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].ClassGuid = QString::fromWCharArray(szClassGuid);
                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_DEVTYPE,
                    &dwPropertyRegDataType, (BYTE*)szDevType,
                    sizeof(szDevType),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].DevType = QString::fromWCharArray(szDevType);
                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_ENUMERATOR_NAME,
                    &dwPropertyRegDataType, (BYTE*)szEnumeratorName,
                    sizeof(szEnumeratorName),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].EnumeratorName = QString::fromWCharArray(szEnumeratorName);
                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_MFG,
                    &dwPropertyRegDataType, (BYTE*)szDeviceManufacturer,
                    sizeof(szDeviceManufacturer),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].DeviceManufacturer = QString::fromWCharArray(szDeviceManufacturer);
                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME,
                    &dwPropertyRegDataType, (BYTE*)szPhysicalDeviceObjectName,
                    sizeof(szPhysicalDeviceObjectName),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].PhysicalDeviceObjectName = QString::fromWCharArray(szPhysicalDeviceObjectName);
                if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_SERVICE,
                    &dwPropertyRegDataType, (BYTE*)szServiceName,
                    sizeof(szServiceName),    // The size, in bytes
                    &dwSize))
                    Info[DeviceIndex].ServiceName = QString::fromWCharArray(szServiceName);
            }

            if (DeviceInfoSet) {
                SetupDiDestroyDeviceInfoList(DeviceInfoSet);
            }


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

        bool WinApiHelper::QueryWTSSession(PWTS_SESSION_INFO  &SessionsBuffer, DWORD &SessionCount)
        {
            return WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &SessionsBuffer, &SessionCount);
        }

        bool WinApiHelper::GetSessionNumberByUserName(QString Username, quint16 &SessionNumber)
        {
            PWTS_SESSION_INFO sessionsBuffer = NULL;
            DWORD sessionCount = 0;
            WTS_SESSION_INFO  wts;
            WTS_INFO_CLASS wic;
            m_logger = spdlog::get("file_logger");

            if(QueryWTSSession(sessionsBuffer, sessionCount))
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
                            m_logger->warn("Username is empty.");
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
                m_logger->error("WTSEnumerateSessions failed. GetLastError: {}", err);
                return false;
            }
        }

        bool WinApiHelper::GetWtsStateBySessionNumber(quint16 SessionNumber, WTS_CONNECTSTATE_CLASS &ConnectionState)
        {
            DWORD bufferSize = 0;
            LPWSTR buffer;
            WTS_CONNECTSTATE_CLASS* ptr_wts_connect_state = NULL;
            if (WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, SessionNumber, WTS_INFO_CLASS::WTSConnectState, reinterpret_cast<LPWSTR*>(&ptr_wts_connect_state), &bufferSize))
            {
                ConnectionState = *ptr_wts_connect_state;
                //SessionState = QString::fromWCharArray(buffer, bufferSize);
                WTSFreeMemory(ptr_wts_connect_state);
                return true;
            }
            else
            {
                return false;
            }
        }

	}
}