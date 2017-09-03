#include "DeviceFactory.h"
#include "USBDevice.h"
#include "RemoteBoxDevice.h"
#include "AnelHome.h"
#include "VoltCraft.h"
#include "RemoteDataConnectLibrary.h"

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
            QVector<std::function<void(void)>> SwitchOnMessage;
            
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

        void DeviceFactory::CreateSwitchOnMessage(const RW::SQL::Peripheral &Device, QVector<std::function<void(void)>> &SwitchOnMessage, QMap<PeripheralType, AbstractDevice*> *m_DeviceList)
        {
            RW::SQL::PeripheralConditionList* list = Device.ConditionList();
            for (quint8 i = 0; i < list->rowCount(); i++)
            {
               RW::SQL::PeripheralCondition* p = list->peripheralCondition(i);
               RW::HW::AbstractDevice* device = m_DeviceList->value(p->DeviceType());
               SwitchOnMessage.append(device->GetCallback(p->TypeOfConnection()));
            }
        }
    }
}