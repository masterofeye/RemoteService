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
            RemoteBoxDevice(PeripheralType DeviceType, QObject *parent = 0);
			virtual ~RemoteBoxDevice();

			bool Initialize();
			bool Reset();
			bool Shutdown();

			RemoteBoxWrapper::Wrapper* GetDevice();
		};
	}
}

