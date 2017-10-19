#include "PowerStripeDevice.h"

namespace RW{
	namespace HW{

        PowerStripeDevice::PowerStripeDevice(PeripheralType DeviceType, QVector<std::function<bool(void)>> SwitchOnCondition, QObject *parent) : AbstractDevice(DeviceType, SwitchOnCondition, parent)
		{
		}


		PowerStripeDevice::~PowerStripeDevice()
		{
		}
	}
}