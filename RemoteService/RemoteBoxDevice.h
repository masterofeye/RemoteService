#pragma once
#include "AbstractDevice.h"
#include "RemoteBoxWrapper.h"

namespace RemoteBoxWrapper
{
	class Wrapper;
}

namespace RW{
	namespace HW{

		class RemoteBoxDevice :
			public AbstractDevice
		{
			Q_OBJECT
			RemoteBoxWrapper::Wrapper* m_Wrapper;
		public:

            RemoteBoxDevice(PeripheralType DeviceType, QVector<std::function<void(void)>> SwitchOnCondition, QObject *parent = 0);
			virtual ~RemoteBoxDevice();

			bool Initialize();
			bool Reset();
            bool Deinitialize();
            virtual void Callback();
            std::function<void(void)> GetCallback(TypeOfElement);
			RemoteBoxWrapper::Wrapper* GetDevice();
        public slots:
        virtual void OnProcessMessage(COM::Message Msg);
		};
	}
}

