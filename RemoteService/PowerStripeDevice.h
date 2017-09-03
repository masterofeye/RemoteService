#pragma once
#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		class PowerStripeDevice :
			public AbstractDevice
		{
		public:
            PowerStripeDevice(PeripheralType DeviceType, QVector<std::function<void(void)>> SwitchOnCondition, QObject *parent = 0);
			~PowerStripeDevice();

            virtual void Callback() = 0;
            virtual std::function<void(void)> GetCallback(TypeOfElement) = 0;

			virtual bool SwitchPort(quint8 Port, PortState St) = 0;
			virtual bool SwitchAll(PortState St) = 0;
			virtual bool GetPortState(quint8 Port, PortState &St) = 0;
        public slots:
            virtual void OnProcessMessage(COM::Message Msg) = 0;
		};
	}
}