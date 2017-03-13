#include "JobScheduler.h"
#include "JobQueue.h"
#include "DeviceManager.h"

namespace RW{
	namespace CORE
	{
		JobScheduler::JobScheduler(HW::DeviceManager* Manager) : m_Queue(new JobQueue(this)),
			m_DeviceManager(Manager)
		{
		}


		void JobScheduler::ProcessJobs()
		{
			if (m_logger == nullptr)
				m_logger = spdlog::get("file_logger");

			Q_ASSERT_X(m_logger == nullptr, "ProcessJobs", "m_logger is null");

			while (m_Process)
			{
				if (m_Queue == nullptr)
					continue;

				if (m_Queue->IsEmpty())
				{
					Sleep(50);
					continue;
				}

				AbstractCommand *cmd = nullptr;
				m_Queue->pop(&cmd);

				if (cmd == nullptr)
				{
					m_logger->error("Command is null!","RemoteService");
					continue;
				}

				//Command emit his ready signal and jobscheduler send the answer back to the client module
				QObject::connect(cmd, &AbstractCommand::finished, this, &JobScheduler::SendAnswerToClient);


				cmd->SetDevice(m_DeviceManager);

				if (!cmd->Execute())
				{
					m_logger->error("Command {} execution failed",cmd->CommandID() );
				}
				
			}
		}

		void JobScheduler::AddNewJob(AbstractCommand* Command)
		{
			if (Command == nullptr)
			{
				m_logger->error("Command is null!");
				return;
			}

			m_Queue->push(Command);
		}
	}
}