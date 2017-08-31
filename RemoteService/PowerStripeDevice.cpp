#include "PowerStripeDevice.h"

namespace RW{
	namespace HW{

        PowerStripeDevice::PowerStripeDevice(PeripheralType DeviceType, QObject *parent) : AbstractDevice(DeviceType,parent)
		{
		}


		PowerStripeDevice::~PowerStripeDevice()
		{
		}
	}
}