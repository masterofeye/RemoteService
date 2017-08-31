#include "DeviceManager.h"
#include "PowerStripeDevice.h"
#include "PowerSupplyDevice.h"
#include "RemoteBoxDevice.h"
#include "VoltCraft.h"
#include "AnelHome.h"
#include "AbstractDevice.h"
#include "RemoteDataConnectLibrary.h"
#include "WinApiDeviceHelper.h"

namespace RW{
	namespace HW{




        DeviceManager::DeviceManager(CORE::ConfigurationManager* CfgManager, QObject *Parent) : QObject(Parent),
            m_DeviceList(new QMap<PeripheralType, AbstractDevice*>()),
			m_State(State::Init),
            m_ConfigManager(CfgManager),
            m_WinHelper(new CORE::WinApiDeviceHelper(this))
		{

		}

		void DeviceManager::SetLogger(std::shared_ptr<spdlog::logger> Logger)
		{
			m_logger = Logger;
		}

		bool DeviceManager::Init()
		{
            QVariant var;
            //Geräteliste durchgehen und jedes Geräte in der Liste, versuchen zu initialisieren und dann zu registrieren.
            m_ConfigManager->GetConfigValue(RW::CORE::ConfigurationName::PeripheralTable, var);
            QList<RW::SQL::Peripheral> list = var.value<QList<RW::SQL::Peripheral>>();
            for each (auto var in list)
            {
                RegisterNewDevice(var);
            }
             
            m_State = State::Running;
			return true;
		}

		bool DeviceManager::DeInit()
		{
            if (m_State == State::Running)
			{
                QMapIterator<PeripheralType, AbstractDevice*> i(*m_DeviceList);
				while (i.hasNext()) {
					i.next().value()->Shutdown();
				}
			}
			return true;
		}

		DeviceManager::~DeviceManager()
		{

		}

        AbstractDevice* DeviceManager::GetDevice(PeripheralType Type)
		{
			return (AbstractDevice*)m_DeviceList->value(Type);
		}

        bool DeviceManager::RegisterNewDevice(RW::SQL::Peripheral& Device)
        {
            if (m_WinHelper->QuerySpecificDevice(Device.HardwareID().first()))
            {
                m_DeviceList->insert(Device.InteralType(), new AnelHome(RW::PeripheralType::PowerStripe_Anel, this));
                return true;
            }
            else if (Device.InteralType() == PeripheralType::PowerSupply_Voltcraft_FPS1136) //Sonderregel Netzteil werden nicht erfasst, durch WindowsHardware
            {
                m_DeviceList->insert(Device.InteralType(), new VoltCraft(RW::PeripheralType::PowerSupply_Voltcraft_FPS1136, this));
                return true;
            }
            else if (Device.InteralType() == PeripheralType::PowerStripe_Anel) //Sonderregel Anel Home wird nicht über WindowsApi erfasst
            {
                m_DeviceList->insert(Device.InteralType(), new AnelHome(RW::PeripheralType::PowerStripe_Anel, this));
                return true;
            }
            else
            {
                return false;
            }
            Device.SetRegistered(true);

            return true;
        }

        bool DeviceManager::DeregisterNewDevice(RW::SQL::Peripheral& Device)
        {
            Device.SetRegistered(false);
            return false;
        }

	}
}
