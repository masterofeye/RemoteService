#include "DeviceFactory.h"
#include "USBDevice.h"
#include "RemoteBoxDevice.h"
#include "AnelHome.h"
#include "VoltCraft.h"
#include "RemoteDataConnectLibrary.h"
#include "spdlog\spdlog.h"

namespace RW{
    namespace HW
    {
        DeviceFactory::DeviceFactory() 
        {
        }


        DeviceFactory::~DeviceFactory()
        {
        }

        AbstractDevice* DeviceFactory::CreateDevice(const RW::SQL::Peripheral &Device, QMap<PeripheralType, AbstractDevice*> *m_DeviceList)
        {
            RW::PeripheralType type = Device.InteralType();
            QVector<std::function<bool(void)>> SwitchOnMessage;
            
            CreateSwitchOnMessage(Device, SwitchOnMessage, m_DeviceList);

            switch (type)
            {
            case RW::PeripheralType::IECube:
            case RW::PeripheralType::MiniCube:
            case RW::PeripheralType::RenesasE1:
            case RW::PeripheralType::SPIGray:
            case RW::PeripheralType::SPIRed:
                return new USBDevice(type, SwitchOnMessage);
            case RW::PeripheralType::LVDSContinentalBox:
                return nullptr;
            case RW::PeripheralType::PowerStripe_Anel_NETPwrCtrl:
                return new AnelHome(type, SwitchOnMessage);
            case RW::PeripheralType::PowerSupply_Voltcraft_FPS1136:
            case RW::PeripheralType::PowerSupply_Voltcraft_PPS11815:
                return new VoltCraft(type, SwitchOnMessage);
            case RW::PeripheralType::RemoteBox:
                return new RemoteBoxDevice(type, SwitchOnMessage);
            case RW::PeripheralType::RPILvdsBox:
                return nullptr;
            default:
                return nullptr;
                break;
            }
        }

        void DeviceFactory::CreateSwitchOnMessage(const RW::SQL::Peripheral &Device, QVector<std::function<bool(void)>> &SwitchOnMessage, QMap<PeripheralType, AbstractDevice*> *m_DeviceList)
        {
            std::shared_ptr<spdlog::logger> m_Logger = spdlog::get("remoteservice");
            RW::SQL::PeripheralConditionList* list = Device.ConditionList();
            QList<RW::SQL::PeripheralCondition*> conditionList;
            //Ausschließen das Fehlermeldung Aufgrund leerer Liste geworfen werden
            if (list->rowCount() == 0)
                return;

            bool isFollowUp = true;

            RW::SQL::PeripheralCondition* p = nullptr;
            for (quint8 i = 0; i < list->rowCount(); i++)
            {
                p = list->peripheralCondition(i);
                RW::HW::AbstractDevice* device = m_DeviceList->value(p->DeviceType());
                if (device != nullptr)
                {
                    SwitchOnMessage.append(device->GetCallback(p->TypeOfConnection(), p->Pin(), p->Port(), p->Ip()));
                    break;
                }
            }

            do
            {
                if (p != nullptr && p->FollowUpCondition().count() != 0)
                {
                    //es gibt noch followUpConditions
                    conditionList = p->FollowUpCondition();
                    //Zunächst wieder die Alternativen prüfen
                    for each (auto var in conditionList)
                    {
                        RW::HW::AbstractDevice* device = m_DeviceList->value(var->DeviceType());
                        if (device != nullptr)
                        {
                            SwitchOnMessage.append(device->GetCallback(p->TypeOfConnection(), p->Pin(), p->Port(), p->Ip()));
                            //Die Alternative ist gültig, somit können wir die nächste FollowUpCondition suchen
                            p = var;
                            break;
                        }
                        else
                        {
                            //Um eine Endlosschleife zu verhindern setzen wir hier die Abbrechbedingung schon mal
                            p = nullptr;
                        }
                    }
                }
                else
                {
                    //es gibt keine FollowUpCondition mehr
                    isFollowUp = false;
                }
            } while (isFollowUp);


            if (SwitchOnMessage.count() == 0)
            {
                m_Logger->critical("No Condition found for the device");
            }
            
        }
    }
}