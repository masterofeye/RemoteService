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
			RemoteBoxDevice(QObject *parent = 0);
			virtual ~RemoteBoxDevice();

			bool Initialize(){ return m_Wrapper->Init(); }
			bool Reset(){ m_Wrapper->DeInit(); return m_Wrapper->Init(); }
			bool Shutdown(){ m_Wrapper->DeInit(); return true; }

			RemoteBoxWrapper::Wrapper* GetDevice();
		};
	}
}

