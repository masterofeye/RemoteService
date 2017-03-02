#include "TCPServer.h"
#include "IPCThread.h"

namespace RW{
	namespace CORE
	{
		TCPServer::TCPServer(QObject *parent) :
			QTcpServer(parent),
			m_logger(spdlog::get("file_logger"))
		{
			//Q_ASSERT_X(m_logger == nullptr, "BasicServer", "m_logger is null");
		}

		void TCPServer::startServer()
		{
			int port = 1234;

			if (!this->listen(QHostAddress::Any, port))
			{
				m_logger->error("Could not start server");
			}
			else
			{
				m_logger->debug("Listening to port ");// << port << "...");
			}
		}

		// This function is called by QTcpServer when a new connection is available. 
		void TCPServer::incomingConnection(qintptr socketDescriptor)
		{
			// We have a new connection
			m_logger->debug(" Connecting...");

			// Every new connection will be run in a newly created thread
			IPCThread *thread = new IPCThread(socketDescriptor, this);

			// connect signal/slot
			// once a thread is not needed, it will be beleted later
			connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

			thread->start();
		}



	}
}