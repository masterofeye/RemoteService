#include "ShutdownHandler.h"

#include "WinApiHelper.h"
#include "DeviceManager.h"
#include "RemoteDataConnectLibrary.h"

namespace RW{
	namespace CORE{

        ShutdownHandler::ShutdownHandler(RW::HW::DeviceManager* Manager, ConfigurationManager* ConfigManager, QString Version, QObject *Parent) : QObject(Parent),
			m_DeviceManager(Manager),
			m_logger(spdlog::get("remoteservice")),
			m_ShutdownTimer(nullptr),
            m_ConfigurationManager(ConfigManager)
		{
            WorkstationKind workstationType = WorkstationKind::NON;
            QVariant type;

            m_ConfigurationManager->GetConfigValue(ConfigurationName::WorkstationType, type);
            workstationType = type.value<RW::WorkstationKind>();

            if (workstationType == WorkstationKind::RemoteWorkstation)
            {
                QVariant timeout;
                m_ConfigurationManager->GetConfigValue(ConfigurationName::RwShutdownTimer, timeout);
                m_Timeout = timeout.toInt();
                m_logger->info("The current shutdown timeout is {}", (int)spdlog::sinks::FilterType::ShutdownHandler, m_Timeout);
            }
            else if (workstationType == WorkstationKind::BackendPC)
            {
                QVariant timeout;
                m_ConfigurationManager->GetConfigValue(ConfigurationName::BeShutdownTimer, timeout);
                m_Timeout = timeout.toInt();
                m_logger->info("The current shutdown timeout is {}", (int)spdlog::sinks::FilterType::ShutdownHandler, m_Timeout);
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
                if (!m_IsRunning)
                    StartShutdownTimer();
                break;
            case COM::MessageDescription::IN_StopShutdownHandler:
                if (m_IsRunning)
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
            m_IsRunning = true;
            m_ShutdownTimer->setSingleShot(true);
			connect(m_ShutdownTimer, &QTimer::timeout, this, &ShutdownHandler::Shutdown);
            m_ShutdownTimer->start(m_Timeout);
            m_logger->info("Shutdown timer started", (int)spdlog::sinks::FilterType::ShutdownHandler);
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG
		}

		void ShutdownHandler::StopShutdownTimer()
		{
			if (m_ShutdownTimer != nullptr && m_ShutdownTimer->isActive())
			{
				m_ShutdownTimer->stop();
                m_logger->info("Shutdown timer stopped.", (int)spdlog::sinks::FilterType::ShutdownHandler);
                m_IsRunning = false;
			}
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG
		}

		void ShutdownHandler::Shutdown()
        {
            if (m_IsRunning)
            {
                WinApiHelper helper;

                //All registered device will be shutdown now
                if (m_DeviceManager != nullptr)
                    m_DeviceManager->DeInit();

                if (helper.Shutdown())
                {
                    m_IsRunning = false;
                    m_ShutdownTimer->stop();
                    emit ShutdownEvt();

                    m_ConfigurationManager->InsertConfigValue(RW::CORE::ConfigurationName::UserName, "Offline");
                    m_ConfigurationManager->InsertConfigValue(RW::CORE::ConfigurationName::WorkstationState, qVariantFromValue(RW::WorkstationState::OFF));
                    m_logger->debug("RemoteService State is : {}", "OFF");
                    m_logger->info("ShutdownHandler: PC shutdown", (int)spdlog::sinks::FilterType::ShutdownHandler);
                }
                else
                {
                    m_logger->info("ShutdownHandler: PC don't shutdown", (int)spdlog::sinks::FilterType::ShutdownHandler);
                }
#ifdef DEBUG
                m_logger->flush();
#endif // DEBUG
            }

		}

	}
}