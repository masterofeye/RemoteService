#pragma once
#include <QtCore>
#include "spdlog\spdlog.h"
#include "Constants.h"



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
			void OnProcessMessage(Util::MessageReceiver Type, Util::Functions Func, QByteArray Report);
		public slots:
			void Shutdown();
		signals:
			void ShutdownEvt();	
			void NewMessage(Util::MessageReceiver Type, Util::Functions Func, QByteArray Data);
		public slots:
			void StartShutdownTimer();
			void StopShutdownTimer();
		};
	}
}