#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		AbstractDevice::AbstractDevice( QObject *parent) : QObject(parent),
			m_DeviceName(""),
			m_Logger(spdlog::get("file_logger"))
		{
		}

		AbstractDevice::AbstractDevice(std::shared_ptr<spdlog::logger> Logger, QObject *parent) : QObject(parent),
			m_DeviceName(""),
			m_Logger(Logger)
		{
		}


		AbstractDevice::~AbstractDevice()
		{
		}
	}
}