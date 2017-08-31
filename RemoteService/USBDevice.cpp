#include "USBDevice.h"
#include <qprocess.h>

#define DEVCON "devcon.exe"
#define RESTART_CMD "restart"
#define REMOVE_CMD "remove"
#define RESCAN_CMD "rescan"

namespace RW{
	namespace HW{

        USBDevice::USBDevice(PeripheralType DeviceType, QObject *Parent) : AbstractDevice(DeviceType, Parent)
		{
		}


		USBDevice::~USBDevice()
		{
		}

		bool USBDevice::ParseStdOut(QString StdOutput)
		{
            return true;
		}

		bool USBDevice::Initialize()
		{
            m_State = State::Init;
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

		bool USBDevice::Shutdown()
		{
            m_State = State::Deinit;
            return true;
		}


	}
}