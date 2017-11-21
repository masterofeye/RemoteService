#pragma once
#include <QtCore>
#include "spdlog\spdlog.h"
#include "Constants.h"
#include "RemoteCommunicationLibrary.h"


namespace RW{
	namespace HW{
		class DeviceManager;
	}

	namespace CORE{
        class ConfigurationManager;
        /*!
        @brief Die Instanz k�mmert sich um das herunterfahren des Rechners, sobald ein Timeot erreicht oder die Schnittstellenfunktion Shutdown aufgerufen wurde.
        @todo Alle Parameter, die aus der Datenbank gelesen werden, sollten �ber eine einheitliche "Update"-Schnittstelle gelesen und gesetzt werden.
        */
		class ShutdownHandler : public QObject
		{
			Q_OBJECT
		private:
			QThread *m_handlerThread;
			std::shared_ptr<spdlog::logger> m_logger;
			QTimer *m_ShutdownTimer;
			long long m_Timeout;
            bool m_IsRunning = false;
            ConfigurationManager* m_ConfigurationManager;
			RW::HW::DeviceManager* m_DeviceManager;

		public:
            ShutdownHandler(RW::HW::DeviceManager* Manager, ConfigurationManager* ConfigManager, QString Version, QObject *Parent = 0);
			~ShutdownHandler();

		public slots:
            /*!
            @brief Sobald die Instanz mit dem CommunicationServer registriert wurde, empf�ngt die Instanz alle Messages. Die Funktion ist dazu verpflichtet, nach der 
            MessageID zu Filtern um so schnell wie m�glich wieder aus 
            @param[in] COM::Message Message Instanz welches durch ein emit von NewMessage gesendet wurde.
            */
            void OnProcessMessage(COM::Message Msg);
            /*!
            @brief F�hrt den Rechner beim Aufruf dieser Funktion herunter. Der Status in der Datenbank wird in dieser Funktion angepasst, um zu gew�hrleisten, dass
            die Status�nderung erfolgt. 
            */
			void Shutdown();
		signals:
			void ShutdownEvt();	
            /*!
            @brief Signal um eine Nachricht aus der aktuellen Instanz zu versenden.
            @param[in] COM::Message Message Objekt, welche alle Informationen enth�lt, f�r die Weiterverarbeitung des Empf�ngers.
            */
			void NewMessage(COM::Message Msg);
		public slots:
            /*!
            @brief Durch den Aufruf des Slot wird der Shutdowntimer gestartet. Sobald der Timer abgelaufen ist, wird der Rechner heruntergefahren. 
            Die Wert des Timeouts wird aktuell im Konstruktor aus der Datenbank gelesehen. 
            */
			void StartShutdownTimer();
            /*!
            @brief Durch den Aufruf des Slot wird der Shutdowntimer gestoppt.
            */
			void StopShutdownTimer();
		};
	}
}