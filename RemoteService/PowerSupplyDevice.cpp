#include "PowerSupplyDevice.h"

namespace RW{
	namespace HW{

        PowerSupplyDevice::PowerSupplyDevice(PeripheralType DeviceType, QVector<std::function<void(void)>> SwitchOnCondition, QObject *Parent) : AbstractDevice(DeviceType, SwitchOnCondition, Parent)
		{
		}


		PowerSupplyDevice::~PowerSupplyDevice()
		{
		}
	}
}