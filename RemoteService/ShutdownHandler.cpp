#include "ShutdownHandler.hpp"
#include "MongoDbClient.hpp"
#include "GeneralObject.hpp"
#include "WinApiHelper.h"
#include "DeviceManager.h"


namespace RW{
	namespace CORE{

#define DEFAULT_SHUTDOWN_TIMEOUT 60*5*60
			namespace HW{
	
	}

		ShutdownHandler::ShutdownHandler(RW::HW::DeviceManager* Manager, QString Version ,QObject *Parent) : QObject(Parent),
			m_DeviceManager(Manager)
		{
			MONGO::GeneralObject *obj = (MONGO::GeneralObject*) MONGO::MongoDbClient::Instance()->GeneralInformation(Version);
			if (obj == nullptr)
			{
				//Set the default timeout value
				m_Timeout = DEFAULT_SHUTDOWN_TIMEOUT;
			}
			else
			{
				m_Timeout = obj->LogoutTimout();
			}
		}


		ShutdownHandler::~ShutdownHandler()
		{
		}

		void ShutdownHandler::StartShutdownTimer()
		{
			if (m_ShutdownTimer == nullptr)
			{
				m_ShutdownTimer = new QTimer(this);
			}
			connect(m_ShutdownTimer, SIGNAL(timeout()), this, SLOT(Shutdown));
			m_ShutdownTimer->start(m_Timeout);
			m_logger->debug("Shutdown timger started");
		}

		void ShutdownHandler::StopShutdownTimer()
		{
			if (m_ShutdownTimer != nullptr && m_ShutdownTimer->isActive())
			{
				m_ShutdownTimer->stop();
				m_logger->debug("Shutdown timer stopped.");
			}
		}

		void ShutdownHandler::Shutdown()
		{
			WinApiHelper helper;

			//All registered device will be shutdown now
			m_DeviceManager->DeInit();

			if (helper.Shutdown())
			{
				m_logger->debug("ShutdownHandler: PC shutdown");
			}
			else
			{
				m_logger->debug("ShutdownHandler: PC don't shutdown");
			}
		}

	}
}