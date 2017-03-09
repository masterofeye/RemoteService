#include "DeviceManager.h"
#include "PowerStripeDevice.h"
#include "PowerSupplyDevice.h"
#include "RemoteBoxDevice.h"
#include "VoltCraft.h"
#include "AnelHome.h"
#include "AbstractDevice.h"

namespace RW{
	namespace HW{
		DeviceManager::DeviceManager(QObject *Parent) : QObject(Parent),
			m_DeviceList(new QMap<DeviceType, AbstractDevice*>()),
			m_State(State::DeInit)
		{

		}

		void DeviceManager::SetLogger(std::shared_ptr<spdlog::logger> Logger)
		{
			m_logger = Logger;
		}

		bool DeviceManager::Init()
		{
			//TODO dies muss durch config gesetzt werden
			m_DeviceList->insert(RW::HW::DeviceManager::DeviceType::PowerStripe, new AnelHome(this));
			//m_DeviceList->insert(RW::HW::DeviceManager::DeviceType::PowerSupply, new VoltCraft(this));
			//m_DeviceList->insert(RW::HW::DeviceManager::DeviceType::RemoteBox, new RemoteBoxDevice(this));


			QMapIterator<DeviceType, AbstractDevice*> i(*m_DeviceList);
			while (i.hasNext()) {
				if (!i.next().value()->Initialize())
				{
					switch (i.key())
					{
					case RW::HW::DeviceManager::DeviceType::PowerSupply:
						m_logger->error("PowerSupply initialisation failed.");
						m_State = State::Failure;
						break;
					case RW::HW::DeviceManager::DeviceType::RemoteBox:
						m_logger->error("RemoteBox initialisation failed.");
						m_State = State::Failure;
						break;
					case RW::HW::DeviceManager::DeviceType::PowerStripe:
						m_logger->error("PowerStripe initialisation failed.");
						m_State = State::Failure;
						break;
					}
					if (m_State == State::Failure)
						return false;
				}

			}
			return true;
		}

		bool DeviceManager::DeInit()
		{
			if (m_State == State::Init)
			{
				QMapIterator<DeviceType, AbstractDevice*> i(*m_DeviceList);
				while (i.hasNext()) {
					i.next().value()->Shutdown();
				}
			}
			return true;
		}

		DeviceManager::~DeviceManager()
		{

		}

		AbstractDevice* DeviceManager::GetDevice(DeviceType Type)
		{
			return (AbstractDevice*)m_DeviceList->value(Type);
		}
	}
}