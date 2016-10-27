#ifndef INTACTIVITYWATCHER_H
#define INTACTIVITYWATCHER_H
#include <QObject>
#include <QTimer>
#include "spdlog\spdlog.h"

namespace RW{

	namespace CORE{
		class AbstractCommand;

		class InactivityWatcher : public QObject
		{
			Q_OBJECT
		private:
			std::shared_ptr<spdlog::logger> m_logger;
			QTimer *m_TimerLogout;
			long long m_Timeout;
		public:
			explicit InactivityWatcher(QString Version, QObject *parent = 0);
			uint GetLastInputTime();

		signals:
			void UserInactive();
		private slots:
			void LogOutUser();
		public slots:
			void StartInactivityObservation();
			void StopInactivityObservation();
			void StopInactivityObservationWithCmd(AbstractCommand* Cmd);
		};
	}
}
#endif // INTACTIVITYWATCHER_H
