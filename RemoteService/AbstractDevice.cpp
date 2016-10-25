#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		AbstractDevice::AbstractDevice(QObject *parent) : QObject(parent),
			m_DeviceName("")
		{
		}


		AbstractDevice::~AbstractDevice()
		{
		}
	}
}