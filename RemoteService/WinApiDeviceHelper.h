#pragma once
#include "qobject.h"
#include <memory>

namespace spdlog{
    class logger;
}


namespace RW{
    namespace CORE{
        struct DeviceInformation
        {
            QString DeviceDescription = "";
            QString HardwareID = "";
            QString Busnummer = "";
            QString FiendlyName = "";
            QString LocationInfo = "";
            QString LocationPaths = "";
            QString Address = "";
            QString BusTypeGuiD = "";
            QString Class = "";
            QString ClassGuid = "";
            QString DevType = "";
            QString EnumeratorName = "";
            QString DeviceManufacturer = "";
            QString PhysicalDeviceObjectName = "";
            QString ServiceName = "";
        };

        /*!
        @brief Der WinApiDeviceHelper ist die Schnittstelle zwischen der Hardwareverwaltung des RemoteServices und der WindowsApi bzw. der Verwaltung der Ger�te unter Windows.
        Ger�teinformationen die von Windows erfragt werden, soll �ber die Klasse erfragt werden.
        In der aktuellen Version, der Klasse, werden die Ger�te die durch den Ger�temanager verf�gbar sind in eine Liste geladen /see QueryDeviceInformation. 
        @version 1.0 
        @autor Ivo Kunadt
        */
        class WinApiDeviceHelper :
            public QObject
        {
        private:
            std::shared_ptr<spdlog::logger> m_Logger;
            QVector<DeviceInformation>* m_Info = nullptr;
        public:
            WinApiDeviceHelper(QObject* Parent);
            ~WinApiDeviceHelper();

            /*
            @brief Schnittstelle des API Helpers, um die ben�tigte Ger�te Information auszulesen.
            */
            void QueryDeviceInformation();

            /*!
            @brief �berpr�ft ob eine Device gerade Aktive �ber die WindowsApi abgefragt werden kann.
            @param DeviceID /see https://msdn.microsoft.com/en-us/library/windows/hardware/ff551963(v=vs.85).aspx
            @return true, sofern das Ger�te �ber die WindowsApi gefunden wurde.
            */
            bool QuerySpecificDevice(QString DeviceId);
        private:

            bool SetupWinApi();
            void QueryDeviceInfo(QString& Info, quint64 DeviceInfoId);
        };

    }

}