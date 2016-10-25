#include "IPCThread.h"
#include "CommandInterpreter.hpp"

namespace RW{
	namespace CORE
	{
		IPCThread::IPCThread(qintptr ID, QObject *parent) :
			QThread(parent),
			m_logger(spdlog::get("file_logger")),
			m_Command(new CommandInterpreter(this))
		{
			this->socketDescriptor = ID;
			Q_ASSERT_X(m_logger == nullptr, "BasicServer", "m_logger is null");
		}

		void IPCThread::run()
		{
			// thread starts here
			qDebug() << " Thread started";

			socket = new QTcpSocket();

			// set the ID
			if (!socket->setSocketDescriptor(this->socketDescriptor))
			{
				// something's wrong, we just emit a signal
				emit error(socket->error());
				return;
			}

			// connect socket and signal
			// note - Qt::DirectConnection is used because it's multithreaded
			//        This makes the slot to be invoked immediately, when the signal is emitted.

			connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
			connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

			// We'll have multiple clients, we want to know which is which
			qDebug() << socketDescriptor << " Client connected";

			// make this thread a loop,
			// thread will stay alive so that signal/slot to function properly
			// not dropped out in the middle when thread dies

			exec();
		}



		void IPCThread::ReadyRead()
		{
			// get the information
			QByteArray Data = socket->readAll();

			m_Command->Interpret(&Data);
			socket->write(Data);
		}

		void IPCThread::Disconnected()
		{
			qDebug() << socketDescriptor << " Disconnected";


			socket->deleteLater();
			exit(0);
		}
	}
}