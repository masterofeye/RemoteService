#pragma once
#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		class PowerStripeDevice :
			public AbstractDevice
		{
		public:
            PowerStripeDevice(PeripheralType DeviceType, QVector<std::function<bool(void)>> SwitchOnCondition, QObject *parent = 0);
			~PowerStripeDevice();

            virtual bool Callback(QString Pin, QString Port, QHostAddress IP) = 0;
            virtual std::function<bool(void)> GetCallback(TypeOfElement, QString, QString, QHostAddress) = 0;

			virtual bool SwitchPort(quint8 Port, PortState St) = 0;
			virtual bool SwitchAll(PortState St) = 0;
			virtual bool GetPortState(quint8 Port, PortState &St) = 0;
        public slots:
            virtual void OnProcessMessage(COM::Message Msg) = 0;
		};
	}
}