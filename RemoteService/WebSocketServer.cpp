#include "WebSocketServer.h"

#include <QtWebSockets/QtWebSockets>
namespace RW{
	namespace CORE{
		WebSocketServer::WebSocketServer(QObject *Parent) : BasicServer(Parent),
			m_WebSocketServer(new QWebSocketServer( QStringLiteral("Echo Server"), QWebSocketServer::NonSecureMode, Parent))
		{
		}


		WebSocketServer::~WebSocketServer()
		{
		}
		
		bool WebSocketServer::Listen(quint16 Port)
		{
			if (!m_WebSocketServer->listen(QHostAddress::Any, Port))
			{
				m_logger->error("WebsocketServer coundn't started.");
				return false;
			}
			else
			{
				m_logger->debug("WebsocketServer started.");
				return true;
			}
		}


		void WebSocketServer::PrepareIncomingConnection()
		{
			QWebSocket *Socket = m_WebSocketServer->nextPendingConnection();
			if (Socket != nullptr)
			{

				WebSocketThread *thread = new WebSocketThread(Socket);
				if (thread != nullptr)
				{
					connect(thread, &WebSocketThread::finished, thread, &WebSocketThread::deleteLater);
					connect(thread, &WebSocketThread::Message, this, &WebSocketServer::OnMessage);
					connect(this, &WebSocketServer::ProcessedMessage, thread, &WebSocketThread::OnProcessedMessage);
					connect(thread, &WebSocketThread::SocketError, thread, &WebSocketThread::OnSocketError);
					thread->start();
				}
				else
				{
					m_logger->error("No thread available. WebSocketThread contructor returns a null_ptr");
				}
			}
			else
			{
				m_logger->error("No client available. nextPendingConnection returns a null_ptr");
			}
		}

		void WebSocketServer::OnDisconnect()
		{
			m_logger->debug("WebSocketServer closes now.");
		}

		void WebSocketServer::OnSocketError(quint16 Error)
		{
			emit SocketError(Error);
		}

		void WebSocketServer::OnMessage(AbstractCommand* Command)
		{
			emit Message(Command);
		}

		void WebSocketServer::OnProcessedMessage(AbstractCommand* Command)
		{
			emit Message(Command);
		}

	}
}
