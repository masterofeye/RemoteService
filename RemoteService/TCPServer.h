#pragma once
#include <QtNetwork>
#include "spdlog\spdlog.h"

namespace RW{
	namespace CORE
	{
		class TCPServer : public QTcpServer
		{
			Q_OBJECT
		private:
			QTcpSocket *socket;
			int socketDescriptor;
			std::shared_ptr<spdlog::logger> m_logger;
		public:
			explicit TCPServer(QObject *parent = 0);
			void startServer();

		protected:
			void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
		};
	}
}