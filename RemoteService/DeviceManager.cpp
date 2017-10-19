#include "DeviceManager.h"
#include "PowerStripeDevice.h"
#include "PowerSupplyDevice.h"
#include "RemoteBoxDevice.h"
#include "VoltCraft.h"
#include "AnelHome.h"
#include "AbstractDevice.h"
#include "RemoteDataConnectLibrary.h"
#include "WinApiDeviceHelper.h"
#include "DeviceFactory.h"

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

        bool caseInsensitiveLessThan(const RW::SQL::Peripheral &s1, const RW::SQL::Peripheral &s2)
        {
            QList<RW::SQL::Peripheral> temp;

            while (!ToSort.isEmpty())
            {
                QList<RW::SQL::Peripheral>::iterator it = ToSort.begin();
                while (it != ToSort.end())
                {
                    it->ConditionList()->peripheralCondition(0)->Priority()
                }
            }
            ToSort = temp;
        }

		bool DeviceManager::Init()
		{
            QVariant var;
            //Geräteliste durchgehen und jedes Geräte in der Liste, versuchen zu initialisieren und dann zu registrieren.
            m_ConfigManager->GetConfigValue(RW::CORE::ConfigurationName::PeripheralTable, var);
            QList<RW::SQL::Peripheral> list = var.value<QList<RW::SQL::Peripheral>>();

            SortList(list);

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
					i.next().value()->Deinitialize();
				}
			}
			return true;
		}

		DeviceManager::~DeviceManager()
		{
            
            if (m_DeviceList != nullptr)
            {
                for each (auto var in *m_DeviceList)
                {
                    if (var != nullptr)
                        delete var;
                }

                m_DeviceList->clear();
                delete m_DeviceList;
            }
		}

        AbstractDevice* DeviceManager::GetDevice(PeripheralType Type)
		{
			return (AbstractDevice*)m_DeviceList->value(Type);
		}



        bool DeviceManager::RegisterNewDevice(QString DeviceName)
        {
            QVariant var;
            //Geräteliste durchgehen und jedes Geräte in der Liste, versuchen zu initialisieren und dann zu registrieren.
            m_ConfigManager->GetConfigValue(RW::CORE::ConfigurationName::PeripheralTable, var);
            QList<RW::SQL::Peripheral> list = var.value<QList<RW::SQL::Peripheral>>();
        
            qSort(list.begin(), list.end(), SortList);

            QListIterator<RW::SQL::Peripheral> i(list);
            while (i.hasNext()) {
                RW::SQL::Peripheral peripheral = i.next();
                if (peripheral.DeviceName() == DeviceName){
                    auto ptrDevice = DeviceFactory::CreateDevice(peripheral,m_DeviceList);
                    if (ptrDevice->Initialize())
                    {
                        m_DeviceList->insert(peripheral.InteralType(), ptrDevice);
                        peripheral.SetRegistered(true);
                    }
                }
            }
            return true;
        }

        bool DeviceManager::RegisterNewDevice(RW::SQL::Peripheral Device)
        {
            bool res = true;
            auto ptrDevice = DeviceFactory::CreateDevice(Device, m_DeviceList);
            if (ptrDevice->Initialize())
            {
                m_DeviceList->insert(Device.InteralType(), ptrDevice);

                Device.SetRegistered(true);
                m_logger->info("Device was registered {}", Device.Description().toStdString());
                m_logger->flush();
            }
            else
            {
                res = false;
            }
            return res;
        }

        bool DeviceManager::DeregisterNewDevice(QString DeviceName)
        {
            QVariant var;
            //Geräteliste durchgehen und jedes Geräte in der Liste, versuchen zu initialisieren und dann zu registrieren.
            m_ConfigManager->GetConfigValue(RW::CORE::ConfigurationName::PeripheralTable, var);
            QList<RW::SQL::Peripheral> list = var.value<QList<RW::SQL::Peripheral>>();

            QListIterator<RW::SQL::Peripheral> i(list);
            while (i.hasNext()) {
                RW::SQL::Peripheral peripheral = i.next();
                if (peripheral.DeviceName() == DeviceName){
                    auto ptrDevice = DeviceFactory::CreateDevice(peripheral, m_DeviceList);
                    ptrDevice->Deinitialize();
                    m_DeviceList->remove(peripheral.InteralType());
                    peripheral.SetRegistered(false);
                }
            }
            return true;
        }
	}
}
