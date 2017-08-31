#pragma once
#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		class PowerStripeDevice :
			public AbstractDevice
		{
		public:
            PowerStripeDevice(PeripheralType DeviceType, QObject *parent = 0);
			~PowerStripeDevice();

			virtual bool SwitchPort(quint8 Port, PortState St) = 0;
			virtual bool SwitchAll(PortState St) = 0;
			virtual bool GetPortState(quint8 Port, PortState &St) = 0;
		};
	}
}