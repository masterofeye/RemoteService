#include "VoltCraft.h"

namespace RW{
	namespace HW{

        VoltCraft::VoltCraft(PeripheralType DeviceType, QObject *Parent) : PowerSupplyDevice(DeviceType, Parent)
		{
		}


		VoltCraft::~VoltCraft()
		{
		}
	}
}
