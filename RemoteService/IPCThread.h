#pragma once

#include <QThread>
#include <QTcpSocket>
#include "spdlog\spdlog.h"

namespace RW{
	namespace CORE
	{
		class CommandInterpreter;
		class IPCThread : public QThread
		{
			Q_OBJECT
		public:
			IPCThread(qintptr ID, QObject *parent = 0);
			void run();
			std::shared_ptr<spdlog::logger> m_logger;
			CommandInterpreter *m_Command;

		signals:
			void error(QTcpSocket::SocketError socketerror);

			public slots:
			void ReadyRead();
			void Disconnected();

		private:
			QTcpSocket *socket;
			qintptr socketDescriptor;
		};
	}
}


