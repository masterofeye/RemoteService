#include "PowerSupplyDevice.h"

namespace RW{
	namespace HW{

        PowerSupplyDevice::PowerSupplyDevice(PeripheralType DeviceType, QObject *Parent) : AbstractDevice(DeviceType, Parent)
		{
		}


		PowerSupplyDevice::~PowerSupplyDevice()
		{
		}
	}
}