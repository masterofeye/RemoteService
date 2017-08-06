#include "DeviceManager.h"
#include "PowerStripeDevice.h"
#include "PowerSupplyDevice.h"
#include "RemoteBoxDevice.h"
#include "VoltCraft.h"
#include "AnelHome.h"
#include "AbstractDevice.h"
#include "WinApiHelper.h"
#include "RemoteDataConnectLibrary.h"

namespace RW{
	namespace HW{




        DeviceManager::DeviceManager(CORE::ConfigurationManager* CfgManager, QObject *Parent) : QObject(Parent),
			m_DeviceList(new QMap<DeviceType, AbstractDevice*>()),
			m_State(State::DeInit),
            m_ConfigManager(CfgManager)
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
            CollectHardwarePeripherie();

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

        bool DeviceManager::CollectHardwarePeripherie()
        {
            bool res = false;

            /*Liste aller verfügbaren Geräte holen*/
            QMap<QString, QVariant> deviceList;
            m_ConfigManager->GetConfigValue(RW::CORE::ConfigurationName::PeripheralTable, deviceList);

            /*Liste erstellen welche HW am Rechner verfügbar ist*/
            QMap<PeripheralType, QVariant> deviceActiveList;
            /*HW auslesen*/
            RW::CORE::WinApiHelper helper;
            QVector<RW::CORE::DeviceInformation> deviceInfoList(20);
            helper.QueryActiveHW(deviceInfoList);
            /*mit möglicher HW vergleichen und in neue liste eintragen*/
            for each (auto var in deviceInfoList)
            {
                if (deviceList.contains(var.HardwareID))
                {
                    QVariant val = deviceList.value(var.HardwareID);
                    RW::SQL::Peripheral peripheral = val.value<RW::SQL::Peripheral>();
                    deviceActiveList.insert(peripheral.Type(), QVariant::fromValue(peripheral));
                }
            }
            return res;
        }

        bool DeviceManager::RegisterNewDevice(QString DeviceName)
        {
            return false;
        }

        bool DeviceManager::DeregisterNewDevice(QString DeviceName)
        {
            return false;
        }
	}
}