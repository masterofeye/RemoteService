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
        @brief Der WinApiDeviceHelper ist die Schnittstelle zwischen der Hardwareverwaltung des RemoteServices und der WindowsApi bzw. der Verwaltung der Geräte unter Windows.
        Geräteinformationen die von Windows erfragt werden, soll über die Klasse erfragt werden.
        In der aktuellen Version, der Klasse, werden die Geräte die durch den Gerätemanager verfügbar sind in eine Liste geladen /see QueryDeviceInformation. 
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
            @brief Schnittstelle des API Helpers, um die benötigte Geräte Information auszulesen.
            */
            void QueryDeviceInformation();

            /*!
            @brief Überprüft ob eine Device gerade Aktive über die WindowsApi abgefragt werden kann.
            @param DeviceID /see https://msdn.microsoft.com/en-us/library/windows/hardware/ff551963(v=vs.85).aspx
            @return true, sofern das Geräte über die WindowsApi gefunden wurde.
            */
            bool QuerySpecificDevice(QString DeviceId);
        private:

            bool SetupWinApi();
            void QueryDeviceInfo(QString& Info, quint64 DeviceInfoId);
        };

    }

}