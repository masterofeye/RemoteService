#pragma once
#include "PowerSupplyDevice.h"

namespace RW{
	namespace HW{

		class VoltCraft :
			public PowerSupplyDevice
        {

		public:
            VoltCraft(PeripheralType DeviceType, QVector<std::function<bool(void)>> SwitchOnCondition, QObject *parent = 0);
			~VoltCraft();

			bool Initialize(){ 
                m_State = State::Init;

                for each (auto var in m_SwitchOnCondition)
                {
                    if (!var())
                        return false;
                }

                m_State = State::Running;
                return true;
                }
			bool Reset(){ return false; }
            bool Deinitialize(){ return false; }

            virtual bool Callback(QString Pin, QString Port, QHostAddress IP);
            virtual std::function<bool(void)> GetCallback(TypeOfElement, QString Pin, QString Port, QHostAddress Ip);

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
