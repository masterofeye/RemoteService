#include "RemoteBoxDevice.h"
#include "RemoteBoxWrapper.h"
#include "RemoteDataConnectLibrary.h"

namespace RW{
	namespace HW{

        RemoteBoxDevice::RemoteBoxDevice(PeripheralType DeviceType, QVector<std::function<void(void)>> SwitchOnCondition, QObject *Parent) : AbstractDevice(DeviceType, SwitchOnCondition, Parent),
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

        void RemoteBoxDevice::OnProcessMessage(COM::Message Msg)
        {
            switch (Msg.MessageID())
            {
            case COM::MessageDescription::IN_REMOTEBOX_ON:
                if ((m_State == State::Deinit || m_State == State::Init) && !Msg.IsProcessed())
                {
                    Initialize();
                    
                }
                else if (Msg.IsProcessed())
                {
                }
                else
                {
                    Msg.SetIsProcessed(true);
                    Msg.SetResult(true);
                    emit NewMessage(Msg);
                }
                break;
            case COM::MessageDescription::IN_REMOTEBOX_OFF:
                if (m_State == State::Running && !Msg.IsProcessed())
                {
                    Deinitialize();
                }
                else if (Msg.IsProcessed())
                {
                }
                break;
            case COM::MessageDescription::IN_REMOTEBOX_RESET:
                if ((m_State == State::Running || m_State == State::Failure) && !Msg.IsProcessed())
                {
                    Reset();
                }
                else if (Msg.IsProcessed())
                {
                }
                break;
            default:
                break;
            }
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

        bool RemoteBoxDevice::Deinitialize(){
			std::string res;
			m_Wrapper->Deinit(res); return true;
		}

        void RemoteBoxDevice::Callback()
        {
        }


        std::function<void(void)> RemoteBoxDevice::GetCallback(TypeOfElement Connection)
        {
            switch (Connection)
            {
            case TypeOfElement::USB:
                return std::bind(&RemoteBoxDevice::Callback, this);
            default:
                break;
            }


        }
	}
}