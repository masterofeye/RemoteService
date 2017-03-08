#ifndef INTACTIVITYWATCHER_H
#define INTACTIVITYWATCHER_H
#include <QObject>
#include <QTimer>
#include "spdlog\spdlog.h"
#include "Constants.h"

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
		signals:
			void UserInactive();
            void NewMessage(Util::Functions MessageType, QByteArray Message);
		public slots:
			void StartInactivityObservation();
			void StopInactivityObservation();
			void StopInactivityObservationWithCmd(AbstractCommand* Cmd);
            void OnProcessMessage(Util::MessageReceiver Type, Util::Functions Func, QByteArray Report);
		};
	}
}
#endif // INTACTIVITYWATCHER_H
