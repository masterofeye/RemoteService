#include "CommandBuilder.h"
#include "DeviceManager.h"

namespace RW{
	namespace CORE
	{
        CommandBuilder::CommandBuilder(QObject *Parent) : AbstractCommandBuilder(Parent),
			m_logger(spdlog::get("file_logger"))
		{
			//Q_ASSERT_X(m_logger == nullptr, "CommandBuilder", "m_logger is null");
		}


		CommandBuilder::~CommandBuilder()
		{
		}

        AbstractCommand* CommandBuilder::CreateCommand(COM::Message Obj)
		{
            switch (Obj.MessageID())
			{
            case RW::COM::MessageDescription::IN_IO:
				return new IOCommand(Obj);
            case RW::COM::MessageDescription::IN_RELAY:
				return new RelayCommand(Obj);
            case RW::COM::MessageDescription::IN_USB:
				return new USBCommand(Obj);
            case RW::COM::MessageDescription::IN_FHOST:
				return nullptr;
            case RW::COM::MessageDescription::IN_ADC:
				return new ADCCommand(Obj);
            case RW::COM::MessageDescription::IN_DAC:
				return new DACCommand( Obj);
            case RW::COM::MessageDescription::IN_UART:
				return nullptr;
				break;
            case RW::COM::MessageDescription::IN_I2C:
				return nullptr;
				break;
            case RW::COM::MessageDescription::IN_POWERSTRIPE:
				return new PowerStripeCommand(Obj);
            case RW::COM::MessageDescription::IN_POWERSUPPLY:
				return new PowerSupplyCommand(Obj);
			default:
				m_logger->warn("No command type was found that match the parameter.");
				return nullptr;
			}
			return nullptr;
		}
	}
}