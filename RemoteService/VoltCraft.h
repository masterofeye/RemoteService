#pragma once
#include "PowerSupplyDevice.h"

namespace RW{
	namespace HW{

		class VoltCraft :
			public PowerSupplyDevice
		{
		public:
			VoltCraft(QObject *parent = 0);
			~VoltCraft();

			bool Initialize(){ return false; }
			bool Reset(){ return false; }
			bool Shutdown(){ return false; }

			bool GetVoltage(double &Value){ return true; }
			bool GetCurrent(double &Value){ return true; }
			bool GetCurrentLimit(double &Value){ return true; }
			bool SetCurrentLimit(double Value){ return true; }
			bool SetVoltage(double Value){ return true; }
		};

	}
}
