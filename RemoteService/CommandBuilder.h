#pragma once
#include <QtCore>
#include "spdlog\spdlog.h"

#include "AbstractCommandBuilder.h"
#include "AbstractCommand.h"


namespace RW{
	namespace HW
	{
		class DeviceManager;
	}

	namespace CORE
	{
		class CommandBuilder : public AbstractCommandBuilder
		{
		private:
			std::shared_ptr<spdlog::logger> m_logger;

		public:
			CommandBuilder(QObject *Parent = nullptr);
			~CommandBuilder();

            AbstractCommand* CreateCommand(CommandIds Cmd, COM::Message Obj);
		};
	}
}
