#include "PowerSupplyDevice.h"

namespace RW{
	namespace HW{

        PowerSupplyDevice::PowerSupplyDevice(PeripheralType DeviceType, QVector<std::function<bool(void)>> SwitchOnCondition, QObject *Parent) : AbstractDevice(DeviceType, SwitchOnCondition, Parent)
		{
		}


		PowerSupplyDevice::~PowerSupplyDevice()
		{
		}
	}
}