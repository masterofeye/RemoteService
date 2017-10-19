#include "VoltCraft.h"

namespace RW{
	namespace HW{

        VoltCraft::VoltCraft(PeripheralType DeviceType, QVector<std::function<bool(void)>> SwitchOnCondition, QObject *Parent) : PowerSupplyDevice(DeviceType, SwitchOnCondition, Parent)
		{
		}


		VoltCraft::~VoltCraft()
		{
		}

        void VoltCraft::OnProcessMessage(COM::Message Msg)
        {
        }

        bool VoltCraft::Callback(QString Pin, QString Port, QHostAddress IP)
        {
            m_Logger->debug("VoltCraft Callback was called");
            return true;
        }
        std::function<bool(void)> VoltCraft::GetCallback(TypeOfElement, QString Pin, QString Port, QHostAddress Ip)
        {
            return std::bind(&VoltCraft::Callback, this, Pin,Port,Ip);
        }
	}
}
