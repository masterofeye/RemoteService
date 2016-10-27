#include "CommandInterpreter.hpp"

namespace RW{
	namespace CORE
	{
		CommandInterpreter::CommandInterpreter(QObject *Parent) : QObject(Parent)
		{
		}


		CommandInterpreter::~CommandInterpreter()
		{
		}

		CmdStatus CommandInterpreter::Interpret(QByteArray *Data)
		{
		//	/********************************************/
		//	/**1-Byte**/

		//	int cmd = Data->at(0);
		//	switch ((Cmd) cmd)
		//	{
		//	case Cmd::nenIO:
		//		break;
		//	case Cmd::nenPower:
		//		break;
		//	case Cmd::nenRelay:
		//		break;
		//	case Cmd::nenSoftware:
		//		break;
		//	case Cmd::nenUSB:
		//		break;
		//	}
			CmdStatus test;
			return test;
		}
	}
}