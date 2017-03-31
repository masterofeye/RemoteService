#include "ShutdownHandler.hpp"

#include "WinApiHelper.h"
#include "DeviceManager.h"

#if defined(DEBUG) && defined(DEBUG_WITHOUT_SHUTDOWN)
#define DEFAULT_SHUTDOWN_TIMEOUT 60*60
#elif defined(DEBUG)
#define DEFAULT_SHUTDOWN_TIMEOUT 60*60*1000
#else
#define DEFAULT_SHUTDOWN_TIMEOUT 1000*60*30
#endif



namespace RW{
	namespace CORE{

		ShutdownHandler::ShutdownHandler(RW::HW::DeviceManager* Manager, QString Version ,QObject *Parent) : QObject(Parent),
			m_DeviceManager(Manager),
			m_logger(spdlog::get("file_logger")),
			m_ShutdownTimer(nullptr)
		{
			if (true)
			{
				//Set the default timeout value
				m_Timeout = DEFAULT_SHUTDOWN_TIMEOUT;
			}
			else
			{
				//m_Timeout = obj->LogoutTimout();
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
			m_ShutdownTimer->start(DEFAULT_SHUTDOWN_TIMEOUT);
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