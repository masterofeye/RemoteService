#include "LocalServer.h"

#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qdatastream.h>

namespace RW{
	namespace CORE{



		LocalServer::LocalServer(QObject *Parent = nullptr) : BasicServer(Parent),
			m_logger(spdlog::get("file_logger"))
		{
			m_Server = new QLocalServer(this);
			m_SocketList = new QMap<QLocalSocket*, QString>();

			//Jeder hat Zugriff auf den Server.
			m_Server->setSocketOptions(QLocalServer::WorldAccessOption);
			//Der Server akzeptiert nur eine Verbindung, das ist der RemoteService
			m_Server->setMaxPendingConnections(1);

			connect(m_Server, &QLocalServer::newConnection, this, &LocalServer::PrepareIncomingConnection);
		}

		LocalServer::~LocalServer()
		{
			m_Server->close();
			delete m_SocketList;
		}

		void LocalServer::PrepareIncomingConnection()
		{
			QLocalSocket* socket = m_Server->nextPendingConnection();

			if (socket->isValid())
			{
				m_logger->debug("New Client is connected to the RemoteService");
				m_SocketList->insert(socket, "");

				connect(socket, SIGNAL(disconnected()), m_Server, SLOT(OnSocketDisconnected()));
				connect(socket, SIGNAL(readyRead()), m_Server, SLOT(OnMessage()));
				connect(socket, SIGNAL(error(QLocalSocket::LocalSocketError)), m_Server, SLOT(OnSocketErrorNative(QLocalSocket::LocalSocketError)));
			}
		}

		bool LocalServer::Listen(QString ChannelName)
		{
			if (m_Server->listen(ChannelName))
			{
				m_logger->debug("LocalServer is started on ChannelName {}", ChannelName.toStdString());
				return true;
			}
			else
			{
				m_logger->error("Can't start local server on ChannelName {}", ChannelName.toStdString());
				return false;
			}
		}
 

		void LocalServer::OnDisconnect()
		{
			QLocalSocket* LocalSocket = (QLocalSocket *)sender();
			QString clientname = m_SocketList->find(LocalSocket).value();
			m_logger->debug("Client disonnected from LocalServer: {}", clientname.toStdString());
		}

		void LocalServer::OnMessage(AbstractCommand* Command)
		{
			QLocalSocket* LocalSocket = (QLocalSocket *)sender();
			QString clientname = m_SocketList->find(LocalSocket).value();
			m_logger->debug("Cmd recieved from: "); // clientname.toStdString();

			QDataStream in(LocalSocket);
			in.setVersion(QDataStream::Qt_5_7);
			LocalSocket->readAll();
		}

		void LocalServer::OnProcessedMessage(AbstractCommand* Command)
		{
		
		}

		void LocalServer::OnSocketError(quint16 Error)
		{
			emit SocketError(Error);
		}

		void LocalServer::OnSocketErrorNative(QLocalSocket::LocalSocketError Error)
		{
			switch (Error)
			{
			case QLocalSocket::ConnectionRefusedError:
				m_logger->error("A LocalServer socket error occoured: {}", "ConnectionRefusedError");
				break;
			case QLocalSocket::PeerClosedError:
				m_logger->error("A LocalServer socket error occoured: {}", "PeerClosedError");
				break;
			case QLocalSocket::ServerNotFoundError:
				m_logger->error("A LocalServer socket error occoured: {}", "ServerNotFoundError");
				break;
			case QLocalSocket::SocketAccessError:
				m_logger->error("A LocalServer socket error occoured: {}", "SocketAccessError");
				break;
			case QLocalSocket::SocketResourceError:
				m_logger->error("A LocalServer socket error occoured:  {}", "SocketResourceError"); 
				break;
			case QLocalSocket::SocketTimeoutError:
				m_logger->error("A LocalServer socket error occoured:  {}", "SocketTimeoutError");
				break;
			case QLocalSocket::DatagramTooLargeError:
				m_logger->error("A LocalServer socket error occoured:  {}", "DatagramTooLargeError");
				break;
			case QLocalSocket::ConnectionError:
				m_logger->error("A LocalServer socket error occoured: {}","ConnectionError"); 
				break;
			case QLocalSocket::UnsupportedSocketOperationError:
				m_logger->error("A LocalServer socket error occoured:  {}", "UnsupportedSocketOperationError");
				break;
			case QLocalSocket::UnknownSocketError:
				m_logger->error("A LocalServer socket error occoured:  {}", "UnknownSocketError");
				break;
			case QLocalSocket::OperationError:
				m_logger->error("A LocalServer socket error occoured:  {}", "OperationError");
				break;
			default:
				break;
			}

			OnSocketError(0);
		}
	}
}