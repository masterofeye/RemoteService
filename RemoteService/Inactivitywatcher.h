#ifndef INTACTIVITYWATCHER_H
#define INTACTIVITYWATCHER_H
#include <QObject>
#include <QTimer>
#include <QTime>
#include <spdlog\spdlog.h>
#include "RemoteCommunicationLibrary.h"
#include "Constants.h"

namespace RW{

	namespace CORE{
		class AbstractCommand;
        class ConfigurationManager;
        /*!
        @brief Diese Klasse k�mmert sich um das �berwachen des Nutzers hinsichtlich der Zeit, die der Rechner in Verwendung ist.
        @todo Alle Parameter, die aus der Datenbank gelesen werden, sollten �ber eine einheitliche "Update"-Schnittstelle gelesen und gesetzt werden.
        */
		class InactivityWatcher : public QObject
		{
			Q_OBJECT
		private:
			std::shared_ptr<spdlog::logger> m_logger;
            ConfigurationManager* m_ConfigManager;
			QTimer *m_TimerLogout;
			long long m_Timeout;
            bool m_isRunning = false;
            QTime m_LogoutTimeStart;
            QTime m_LogoutTimeEnd;
		public:
            explicit InactivityWatcher(QString Version, ConfigurationManager* Configmanager, QObject *parent = 0);
        private:
            /*!
            @brief Sorgt daf�r, dass der User ausgelogt wird. Dabei wird zun�chst eine Message gesendet um den Status auch an andere Programmteile zu verschicken
            @return void
            */
            void LogOutUserImmediately();
        signals:
            /*!
            @brief Signal um eine Nachricht aus der aktuellen Instanz zu versenden.
            @param COM::Message Message Objekt, welche alle Informationen enth�lt, f�r die Weiterverarbeitung des Empf�ngers.
            */
			void NewMessage(COM::Message Msg);
		public slots:
			void StartInactivityObservation();
			void StopInactivityObservation();
			void StopInactivityObservationWithCmd(AbstractCommand* Cmd);
            /*!
            @brief Sobald die Instanz mit dem CommunicationServer registriert wurde, empf�ngt die Instanz alle Messages. Die Funktion ist dazu verpflichtet, nach der
            MessageID zu Filtern um so schnell wie m�glich wieder aus
            @param[in] COM::Message Message Instanz welches durch ein emit von NewMessage gesendet wurde.
            */
            void OnProcessMessage(COM::Message Msg);

        private slots:
            /*!
            @brief Durch den Aufruf des Slot wird der Timeout f�r den Logout gestartet. Sobald der Timer abgelaufen ist, wird der User ausgeloggt.
            Die Wert des Timeouts wird aktuell im Konstruktor aus der Datenbank gelesehen.
            */
            void OnTimeoutStop();
            /*!
            @brief Durch den Aufruf des Slot wird der Timeout f�r den Logout gestoppt.
            */
            void OnTimeoutStart();

		};
	}
}
#endif // INTACTIVITYWATCHER_H
