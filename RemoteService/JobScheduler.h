#pragma once
#include "CommandBuilder.h"
#include <RemoteCommunicationLibrary.h>
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
            CommandBuilder* m_Builder;
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
        void AddNewJob(COM::Message Msg);
        void SendAnswerToClient(COM::Message Msg){ emit SendAnswer(Msg); }
		signals:
			void Finished();
            void SendAnswer(COM::Message Msg);
		};

	}
}