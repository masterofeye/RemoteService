#pragma once
#include <QtCore>
#include "spdlog\spdlog.h"




namespace RW{
	namespace HW{
		class DeviceManager;
	}

	namespace CORE{
		class ShutdownHandler : public QObject
		{
			Q_OBJECT
		private:
			QThread *m_handlerThread;
			std::shared_ptr<spdlog::logger> m_logger;
			QTimer *m_ShutdownTimer;
			long long m_Timeout;

			RW::HW::DeviceManager* m_DeviceManager;

		public:
			ShutdownHandler(RW::HW::DeviceManager* Manager, QString Version, QObject *Parent = 0);
			~ShutdownHandler();

		public slots:
			void Shutdown();
		signals:
			void ShutdownEvt();
		public slots:
			void StartShutdownTimer();
			void StopShutdownTimer();
		};
	}
}