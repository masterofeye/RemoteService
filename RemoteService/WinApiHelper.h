#pragma once
#include "qobject.h"
#include "spdlog\spdlog.h"

#include <Wtsapi32.h>

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
        private: 
            /*!
            @brief Liefer eine Liste an allen Sessions zurück
            @param [out] SessionsBuffer Liste an sessions.
            @param [out] Anzahl an Sessions.
            */
            bool QueryWTSSession(PWTS_SESSION_INFO  &SessionsBuffer, DWORD &SessionCount);
		public:
			WinApiHelper();
			~WinApiHelper();

            /*!
            @brief Gibt die Session des WTS zurück, welche dem Usernamen zugeordnet ist. 
            @param [in] Username Name des User für die Suche nach der passenden Session.
            @param [out] SessionNumber Nummer der Session die mit der Username verknüpft ist.
            */
            bool GetSessionNumberByUserName(QString Username, quint16 &SessionNumber);

            /*!
            @brief Gibt den Status der Session zurück
            @param [in] SessionNumber Nummer der Session
            @param [out] SessionState Aktueller Status der Session
            */
            bool GetWtsStateBySessionNumber(quint16 SessionNumber, WTS_CONNECTSTATE_CLASS &ConnectionState);


			bool QueryActiveSession(quint64 &SessioNumber);
            HANDLE WinApiHelper::GetCurrentUserToken();
            bool WinApiHelper::CreateProcessAsCurrentUser(QString Programm, QString Arguments);
			static bool ReturnCurrentUser(QString &Username);
            bool QueryActiveHW(QVector<DeviceInformation> &Info);
            bool Shutdown();


		};
	}
}


