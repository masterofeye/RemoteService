#pragma once
#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		class PowerSupplyDevice :
			public AbstractDevice
		{
		public:
			PowerSupplyDevice(QObject *parent = 0);
			virtual ~PowerSupplyDevice();
			
			virtual bool GetVoltage(double &Value) = 0;
			virtual bool GetCurrent(double &Value) = 0;
			virtual bool GetCurrentLimit(double &Value) = 0;
			virtual bool SetCurrentLimit(double Value) = 0;
			virtual bool SetVoltage(double Value) = 0;
		};
	}
}
