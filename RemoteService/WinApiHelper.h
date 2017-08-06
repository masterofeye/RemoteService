#pragma once
#include "qobject.h"



#include "spdlog\spdlog.h"



namespace RW
{
	namespace CORE
	{ 
        struct DeviceInformation
        {
            QString DeviceDescription = "";
            QString HardwareID = "";
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


		class WinApiHelper :
			public QObject
		{
		private: 
			std::shared_ptr<spdlog::logger> m_logger;
		public:
			WinApiHelper();
			~WinApiHelper();

			bool QueryActiveSession(quint64 &SessioNumber);
            HANDLE WinApiHelper::GetCurrentUserToken();
            bool WinApiHelper::CreateProcessAsCurrentUser(QString Programm, QString Arguments);
			static bool ReturnCurrentUser(QString &Username);
            bool QueryActiveHW(QVector<DeviceInformation> &Info);
            bool Shutdown();
		};
	}
}


