#include "WinApiHelper.h"

#include <Wtsapi32.h>
#include <userenv.h>
#include <windows.h> 
#include <lm.h>
#include <SetupAPI.h>
#include <devpkey.h>
#include <devguid.h>

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

        bool WinApiHelper::QueryActiveHW()
        {
            HDEVINFO DeviceInfoSet;
            SP_DEVINFO_DATA DeviceInfoData;
			DEVPROPTYPE devicePropertyType;
			DWORD error, dwPropertyRegDataType, dwSize;
			TCHAR szDesc[1024], szHardwareIDs[4096];
			GUID *guidDev = (GUID*)&GUID_DEVCLASS_USB;
			DeviceInfoSet = SetupDiGetClassDevs(guidDev, NULL, NULL, DIGCF_PRESENT);

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
	}
}