#pragma once
#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		class PowerSupplyDevice :
			public AbstractDevice
		{
		public:
            PowerSupplyDevice(PeripheralType DeviceType, QVector<std::function<bool(void)>> SwitchOnCondition, QObject *parent = 0);
			virtual ~PowerSupplyDevice();

            virtual bool Callback(QString Pin, QString Port, QHostAddress IP) = 0;
            virtual std::function<bool(void)> GetCallback(TypeOfElement, QString, QString, QHostAddress) = 0;
			
			virtual bool GetVoltage(double &Value) = 0;
			virtual bool GetCurrent(double &Value) = 0;
			virtual bool GetCurrentLimit(double &Value) = 0;
			virtual bool SetCurrentLimit(double Value) = 0;
			virtual bool SetVoltage(double Value) = 0;
        public slots:
            virtual void OnProcessMessage(COM::Message Msg) = 0;
		};
	}
}
