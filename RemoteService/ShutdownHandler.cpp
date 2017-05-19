#include "ShutdownHandler.hpp"

#include "WinApiHelper.h"
#include "DeviceManager.h"
#include "RemoteDataConnectLibrary.h"

namespace RW{
	namespace CORE{

        ShutdownHandler::ShutdownHandler(RW::HW::DeviceManager* Manager, ConfigurationManager* ConfigManager, QString Version, QObject *Parent) : QObject(Parent),
			m_DeviceManager(Manager),
			m_logger(spdlog::get("file_logger")),
			m_ShutdownTimer(nullptr),
            m_ConfigurationManager(ConfigManager)
		{
            WorkstationKind workstationType = WorkstationKind::NON;
            QVariant type;
            m_ConfigurationManager->GetConfigValue(ConfigurationName::WorkstationType, type);
            workstationType = type.value<WorkstationKind>();

            if (workstationType == WorkstationKind::RemoteWorkstation)
            {
                QVariant timeout;
                m_ConfigurationManager->GetConfigValue(ConfigurationName::RwShutdownTimer,timeout);
                m_Timeout = timeout.toInt();
            }
            else if (workstationType == WorkstationKind::BackendPC)
            {
                QVariant timeout;
                m_ConfigurationManager->GetConfigValue(ConfigurationName::BeShutdownTimer, timeout);
                m_Timeout = timeout.toInt();
            }
		}

		ShutdownHandler::~ShutdownHandler()
		{
		}

        void ShutdownHandler::OnProcessMessage(COM::Message Msg)
        {
            switch (Msg.MessageID())
            {
            case COM::MessageDescription::IN_StartShutdownHandler:
                if(!isRunning)
                    StartShutdownTimer();
                break;
            case COM::MessageDescription::IN_StopShutdownHandler:
                if(isRunning)
                    StopShutdownTimer();
                break;
            default:
                break;
            }
        }

		void ShutdownHandler::StartShutdownTimer()
		{
			if (m_ShutdownTimer == nullptr)
			{
				m_ShutdownTimer = new QTimer(this);
			}
            isRunning = true;
			connect(m_ShutdownTimer, &QTimer::timeout, this, &ShutdownHandler::Shutdown);
            m_ShutdownTimer->start(m_Timeout);
			m_logger->debug("Shutdown timger started");
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG
		}

		void ShutdownHandler::StopShutdownTimer()
		{
			if (m_ShutdownTimer != nullptr && m_ShutdownTimer->isActive())
			{
				m_ShutdownTimer->stop();
				m_logger->debug("Shutdown timer stopped.");
			}
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG
		}

		void ShutdownHandler::Shutdown()
		{
			WinApiHelper helper;

			//All registered device will be shutdown now
			//m_DeviceManager->DeInit();

#ifdef DEBUG_WITHOUT_SHUTDOWN
			//This is for testing propes of the shutdown mechanism. 
			//After the execution of this statment the mechanism is broken because we wont get back in initial state without restart.
			m_logger->debug("ShutdownHandler: PC shutdown");
			m_ShutdownTimer->stop();
			emit ShutdownEvt();
#else
			if (helper.Shutdown())
			{
				m_ShutdownTimer->stop();
				emit ShutdownEvt();
				m_logger->debug("ShutdownHandler: PC shutdown");
			}
			else
			{
				m_logger->debug("ShutdownHandler: PC don't shutdown");
			}
#endif // DEBUG
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG
		}

	}
}