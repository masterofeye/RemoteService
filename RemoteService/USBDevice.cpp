#include "USBDevice.h"
#include <qprocess.h>

#define DEVCON "devcon.exe"
#define RESTART_CMD "restart"
#define REMOVE_CMD "remove"
#define RESCAN_CMD "rescan"

namespace RW{
	namespace HW{

        USBDevice::USBDevice(PeripheralType DeviceType, QVector<std::function<void(void)>> SwitchOnCondition, QObject *Parent) : AbstractDevice(DeviceType, SwitchOnCondition, Parent)
		{
		}


		USBDevice::~USBDevice()
		{
		}

        void USBDevice::OnProcessMessage(COM::Message Msg)
        {
            switch (Msg.MessageID())
            {
            case COM::MessageDescription::IN_USB_SWITCH_ON:
                if ((m_State == State::Deinit || m_State == State::Init) && !Msg.IsProcessed())
                {
                    Initialize();
                }
                break;
            case COM::MessageDescription::IN_USB_SWITCH_OFF:
                if (m_State == State::Running && !Msg.IsProcessed())
                {
                    Deinitialize();
                }
                break;
            case COM::MessageDescription::IN_USB_SWITCH_RESET:
                if ((m_State == State::Running || m_State == State::Failure) && !Msg.IsProcessed())
                {
                    Reset();
                }
                break;
            default: 
                break;
            }
        }

        void USBDevice::Callback()
        {

        }
        std::function<void(void)> USBDevice::GetCallback(TypeOfElement)
        {
            return nullptr;
        }

		bool USBDevice::ParseStdOut(QString StdOutput)
		{
            return true;
		}

		bool USBDevice::Initialize()
		{
            m_State = State::Init;
            
            for each (auto var in m_SwitchOnCondition)
            {
                var();
            }

            m_State = State::Running;
            return true;
		}

		bool USBDevice::Reset()
		{
			QProcess devcon;
			devcon.setProgram(DEVCON);
			devcon.arguments() << RESTART_CMD << m_HardwareID;

			devcon.start();
			devcon.waitForFinished(); // sets current thread to sleep and waits for pingProcess end
			if (ParseStdOut(devcon.readAllStandardOutput()))
			{
				//Geräte wurde erfolgreich zurückgesetzt.
                m_State = State::Failure;
				return true;
			}

			/*zweiter Versuch, nun wird versucht das Gerät zu entfernen und danach einen rescan durchzuführen*/

			devcon.arguments().clear();
			devcon.arguments() << REMOVE_CMD << m_HardwareID;
			devcon.start();
			devcon.waitForFinished(); // sets current thread to sleep and waits for pingProcess end
			if (!ParseStdOut(devcon.readAllStandardOutput()))
			{
				//Geräte wurde nicht erfolgreich entfernt
                m_State = State::Failure;
				return false;
			}

			devcon.arguments().clear();
			devcon.arguments() << RESCAN_CMD;
			devcon.start();
            m_State = State::Running;
            return true;
		}

        bool USBDevice::Deinitialize()
		{
            m_State = State::Deinit;
            return true;
		}


	}
}