#pragma once
#include "PowerSupplyDevice.h"

namespace RW{
	namespace HW{

		class VoltCraft :
			public PowerSupplyDevice
        {

		public:
            VoltCraft(PeripheralType DeviceType, QVector<std::function<void(void)>> SwitchOnCondition, QObject *parent = 0);
			~VoltCraft();

			bool Initialize(){ return false; }
			bool Reset(){ return false; }
            bool Deinitialize(){ return false; }

            virtual void Callback();
            virtual std::function<void(void)> GetCallback(TypeOfElement);

			bool GetVoltage(double &Value){ Q_UNUSED(Value) return true; }
			bool GetCurrent(double &Value){ Q_UNUSED(Value) return true; }
			bool GetCurrentLimit(double &Value){ Q_UNUSED(Value) return true; }
			bool SetCurrentLimit(double Value){ Q_UNUSED(Value) return true; }
			bool SetVoltage(double Value){ Q_UNUSED(Value) return true; }
         public slots:
         virtual void OnProcessMessage(COM::Message Msg);
		};

	}
}
