#pragma once
#include <QtCore>

namespace RW{
	namespace CORE
	{
		enum class ReturnVal
		{
			nenSuccess,
			nenFailure
		};

		struct CmdStatus
		{
			ReturnVal Val;
			QString Message;
		};

		enum class Command
		{
			Switch_ON,
			Switch_OFF,
			Switch_Toggle,
			SetValue,
			GetValue,

		};

		struct Cmd
		{
			Command cmd;
			
		};




		class CommandInterpreter : QObject
		{
			Q_OBJECT
		public:
			CommandInterpreter(QObject *parent = 0);
			~CommandInterpreter();

			CmdStatus CommandInterpreter::Interpret(QByteArray *Data);
		};
	}
}

