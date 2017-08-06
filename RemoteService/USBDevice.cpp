#include "USBDevice.h"

#define DEVCON "devcon.exe"
#define RESTART_CMD "restart"
#define REMOVE_CMD "remove"
#define RESCAN_CMD "rescan"

namespace RW{
	namespace HW{

		USBDevice::USBDevice(QObject *Parent) : AbstractDevice(Parent)
		{
		}


		USBDevice::~USBDevice()
		{
		}

		bool USBDevice::ParseStdOut(QString StdOutput)
		{
		
		}

		bool USBDevice::Initialize()
		{

		}

		bool USBDevice::Reset()
		{
			QProcess devcon;
			devcon.program = DEVCON;
			devcon.arguments() << RESTART_CMD << m_HardwareID;

			devcon.start();
			devcon.waitForFinished(); // sets current thread to sleep and waits for pingProcess end
			if (ParseStdOut(devcon.readAllStandardOutput()))
			{
				//Geräte wurde erfolgreich zurückgesetzt.
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
				return false;
			}

			devcon.arguments().clear();
			devcon.arguments() << RESCAN_CMD;
			devcon.start();
		}

		bool USBDevice::Shutdown()
		{

		}


	}
}