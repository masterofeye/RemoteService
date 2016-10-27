#pragma once
#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		class PowerStripeDevice :
			public AbstractDevice
		{
		public:
			PowerStripeDevice(QObject *parent = 0);
			~PowerStripeDevice();

			virtual bool SwitchPort(quint8 Port, State St) = 0;
			virtual bool SwitchAll(State St) = 0;
			virtual bool GetPortState(quint8 Port, State &St) = 0;
		};
	}
}