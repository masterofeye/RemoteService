#include "WinApiDeviceHelper.h"
#include <spdlog\spdlog.h>
#include <windows.h> 
#include <SetupAPI.h>
#include <devguid.h>
#include <qvector.h>

namespace RW{
    namespace CORE{
        static HDEVINFO DeviceInfoSet;
        static SP_DEVINFO_DATA DeviceInfoData;


        WinApiDeviceHelper::WinApiDeviceHelper(QObject* Parent = nullptr) : QObject(Parent),
            m_Info(new QVector<DeviceInformation>()),
            m_Logger(spdlog::get("remoteservice"))
        {
        }


        WinApiDeviceHelper::~WinApiDeviceHelper()
        {
            if (m_Info != nullptr)
            {
                m_Info->clear();
                delete m_Info;
            }
        }

        bool WinApiDeviceHelper::SetupWinApi()
        {
            //GUID *guidDev = (GUID*)&GUID_DEVCLASS_USB;
            DeviceInfoSet = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);

            if (DeviceInfoSet == INVALID_HANDLE_VALUE)
            {
                m_Logger->error("Unable to create device information set {}", GetLastError());
                return false;
            }

            ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
            DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        }

        void WinApiDeviceHelper::QueryDeviceInformation()
        {
            if (!SetupWinApi())
                return;
            quint16 DeviceIndex = 0;
            while (SetupDiEnumDeviceInfo(DeviceInfoSet, DeviceIndex, &DeviceInfoData))
            {
                if (DeviceIndex > m_Info->count() - 10)
                    m_Info->resize(DeviceIndex + 10);

                QueryDeviceInfo((*m_Info)[DeviceIndex].DeviceDescription, SPDRP_DEVICEDESC);
                QueryDeviceInfo((*m_Info)[DeviceIndex].HardwareID, SPDRP_HARDWAREID);
                QueryDeviceInfo((*m_Info)[DeviceIndex].Busnummer, SPDRP_BUSNUMBER);
                QueryDeviceInfo((*m_Info)[DeviceIndex].FiendlyName, SPDRP_FRIENDLYNAME);
                QueryDeviceInfo((*m_Info)[DeviceIndex].LocationInfo, SPDRP_LOCATION_INFORMATION);
                QueryDeviceInfo((*m_Info)[DeviceIndex].Address, SPDRP_ADDRESS);
                QueryDeviceInfo((*m_Info)[DeviceIndex].Class, SPDRP_CLASS);
                QueryDeviceInfo((*m_Info)[DeviceIndex].BusTypeGuiD, SPDRP_BUSTYPEGUID);
                QueryDeviceInfo((*m_Info)[DeviceIndex].ClassGuid, SPDRP_CLASSGUID);
                QueryDeviceInfo((*m_Info)[DeviceIndex].DevType, SPDRP_DEVTYPE);
                QueryDeviceInfo((*m_Info)[DeviceIndex].EnumeratorName, SPDRP_ENUMERATOR_NAME);
                QueryDeviceInfo((*m_Info)[DeviceIndex].DeviceManufacturer, SPDRP_MFG);
                QueryDeviceInfo((*m_Info)[DeviceIndex].PhysicalDeviceObjectName, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME);
                QueryDeviceInfo((*m_Info)[DeviceIndex].ServiceName, SPDRP_SERVICE);

                DeviceIndex++;
            }
        }

        bool WinApiDeviceHelper::QuerySpecificDevice(QString DeviceId)
        {
            DeviceId = DeviceId.trimmed();
            std::wstring  test = DeviceId.toStdWString();
            HDEVINFO hinfo = SetupDiGetClassDevs(NULL, test.c_str(), NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
            if (hinfo == INVALID_HANDLE_VALUE)
            {
                m_Logger->error("QuerySpecificDevice: invalid device instance. {}", GetLastError());
                return false;
            }
            return true;
        }


        void  WinApiDeviceHelper::QueryDeviceInfo(QString& Info, quint64 DeviceInfoId)
        {
            TCHAR value[4096];
            DWORD dwSize = 0, dwPropertyRegDataType = 0;
            if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, DeviceInfoId,
                &dwPropertyRegDataType, (BYTE*)value,
                sizeof(value),    // The size, in bytes
                &dwSize))
                Info = QString::fromWCharArray(value);
        }
    }
}