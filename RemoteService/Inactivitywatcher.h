#ifndef INTACTIVITYWATCHER_H
#define INTACTIVITYWATCHER_H
#include <QObject>
#include <QTimer>
#include <spdlog\spdlog.h>
#include "RemoteCommunicationLibrary.h"
#include "Constants.h"

namespace RW{

	namespace CORE{
		class AbstractCommand;
        class ConfigurationManager;
		class InactivityWatcher : public QObject
		{
			Q_OBJECT
		private:
			std::shared_ptr<spdlog::logger> m_logger;
            ConfigurationManager* m_ConfigManager;
			QTimer *m_TimerLogout;
			long long m_Timeout;
            bool m_isRunning = false;
		public:
            explicit InactivityWatcher(QString Version, ConfigurationManager* Configmanager, QObject *parent = 0);
        private:
            void LogOutUserImmediately();
		signals:
			void UserInactive();
			void NewMessage(COM::Message Msg);
		public slots:
			void StartInactivityObservation();
			void StopInactivityObservation();
			void StopInactivityObservationWithCmd(AbstractCommand* Cmd);
            void OnProcessMessage(COM::Message Msg);

        private slots:
            void OnTimeoutStop();
            void OnTimeoutStart();

		};
	}
}
#endif // INTACTIVITYWATCHER_H
