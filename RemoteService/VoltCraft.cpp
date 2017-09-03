#include "VoltCraft.h"

namespace RW{
	namespace HW{

        VoltCraft::VoltCraft(PeripheralType DeviceType, QVector<std::function<void(void)>> SwitchOnCondition, QObject *Parent) : PowerSupplyDevice(DeviceType, SwitchOnCondition, Parent)
		{
		}


		VoltCraft::~VoltCraft()
		{
		}

        void VoltCraft::OnProcessMessage(COM::Message Msg)
        {
        }

        void VoltCraft::Callback()
        {

        }
        std::function<void(void)> VoltCraft::GetCallback(TypeOfElement)
        {
            return nullptr;
        }
	}
}
