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

        AbstractCommand* CommandBuilder::CreateCommand(CommandIds Cmd, COM::Message Obj)
		{
			switch (Cmd)
			{
			case RW::CORE::CommandIds::IO:
				return new IOCommand(Obj);
			case RW::CORE::CommandIds::RELAY:
				return new RelayCommand(Obj);
			case RW::CORE::CommandIds::USB:
				return new USBCommand(Obj);
			case RW::CORE::CommandIds::FHOST:
				return nullptr;
			case RW::CORE::CommandIds::ADC:
				return new ADCCommand(Obj);
			case RW::CORE::CommandIds::DAC:
				return new DACCommand( Obj);
			case RW::CORE::CommandIds::UART:
				return nullptr;
				break;
			case RW::CORE::CommandIds::I2C:
				return nullptr;
				break;
			case RW::CORE::CommandIds::CFG:
				break;
			case RW::CORE::CommandIds::POWERSTRIPE:
				return new PowerStripeCommand(Obj);
			case RW::CORE::CommandIds::POWERSUPPLY:
				return new PowerSupplyCommand(Obj);
			case RW::CORE::CommandIds::FLASH:
				return new FlashCommand(Obj);
			case RW::CORE::CommandIds::LOGOUT:
				return new LogOutCommand(Obj);
			case RW::CORE::CommandIds::SHUTDOWN:
				return new ShutdownCommand(Obj);
			default:
				m_logger->warn("No command type was found that match the parameter.");
				return nullptr;
			}
			return nullptr;
		}
	}
}