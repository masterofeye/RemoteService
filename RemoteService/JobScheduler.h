#pragma once
#include <QtCore>
#include "spdlog\spdlog.h"


namespace RW{
	namespace HW
	{
		class DeviceManager;
	}
	namespace CORE
	{
		class JobQueue;
		class AbstractCommand;

		class JobScheduler : public QThread
		{
			Q_OBJECT
		private: 
			std::shared_ptr<spdlog::logger> m_logger;
			HW::DeviceManager* m_DeviceManager;
			JobQueue* m_Queue;
			bool m_Process;
		public:
			JobScheduler(HW::DeviceManager* Manager);
			~JobScheduler()
			{
			}

			void run() Q_DECL_OVERRIDE{ 
				ProcessJobs();
				emit Finished();
			}


			void ProcessJobs();

		public slots:
			void AddNewJob(AbstractCommand* Command);
			void SendAnswerToClient(AbstractCommand* Command){ emit SendAnswer(Command); }
		signals:
			void Finished();
			void SendAnswer(AbstractCommand* Command);
		};

	}
}