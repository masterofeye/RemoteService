#include "RemoteBoxDevice.h"
#include "RemoteBoxWrapper.h"

namespace RW{
	namespace HW{

        RemoteBoxDevice::RemoteBoxDevice(PeripheralType DeviceType, QObject *Parent) : AbstractDevice(DeviceType, Parent),
			m_Wrapper(new RemoteBoxWrapper::Wrapper())
		{
		}


		RemoteBoxDevice::~RemoteBoxDevice()
		{
		}

		RemoteBoxWrapper::Wrapper* RemoteBoxDevice::GetDevice()
		{
			return m_Wrapper;
		}

		bool RemoteBoxDevice::Initialize(){
			std::string res;
			bool success = m_Wrapper->Init(res);
			if (!success)
				m_Logger->error("RemoteBox couldn't initialised: {}",res);
			return success;
		}

		bool RemoteBoxDevice::Reset(){
			std::string res;
			m_Wrapper->Deinit(res);
			bool success = m_Wrapper->Init(res);
			if (!success)
				m_Logger->error("RemoteBox reset failed: {}", res);
			return success;
		}

		bool RemoteBoxDevice::Shutdown(){ 
			std::string res;
			m_Wrapper->Deinit(res); return true;
		}
	}
}