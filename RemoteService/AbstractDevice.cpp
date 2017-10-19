#include "AbstractDevice.h"
#include "RemoteDataConnectLibrary.h"

namespace RW{
	namespace HW{

        AbstractDevice::AbstractDevice(PeripheralType DeviceType, QVector<std::function<bool(void)>> SwitchOnCondition, QObject *parent) : QObject(parent),
			m_DeviceName(""),
			m_Logger(spdlog::get("remoteservice")),
            m_DeviceType(DeviceType),
            m_SwitchOnCondition(SwitchOnCondition)
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