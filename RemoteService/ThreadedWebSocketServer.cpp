#include "ThreadedWebSocketServer.h"
#include "AbstractCommand.h"

#include <QtWebSockets/QtWebSockets>

namespace RW
{
	namespace CORE
	{
		WebSocketThread::WebSocketThread(QObject *Parent) : QThread(Parent),
			m_WebSocketServer(new QWebSocketServer(),
			m_logger(spdlog::get("file_logger"))
		{
			connect(m_WebSocketServer, &QWebSocketServer::newConnection, this, &WebSocketThread::IncomingConnection);
			connect(m_WebSocketServer, &QWebSocketServer::closed, this, &WebSocketThread::Close);
		}

		void WebSocketThread::StartServer(quint16 Port)
		{
			if (m_WebSocketServer->listen(QHostAddress::Any, Port)) {
				m_logger->debug("WebsocketServer started.");
			}
			else
			{
				m_logger->error("Websocketserver couldn't started");
			}
		};

		void WebSocketThread::IncomingConnection()
		{
			QWebSocket *pSocket = m_WebSocketServer->nextPendingConnection();
			connect(pSocket, &QWebSocket::textMessageReceived, this, &WebSocketThread::ProcessTextMessage);
			connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WebSocketThread::ProcessBinaryMessage);
			connect(pSocket, &QWebSocket::disconnected, this, &WebSocketThread::SocketDisconnected);
			m_ClientList << pSocket;
		}

		void WebSocketThread::Close()
		{
		
		}

		void WebSocketThread::ProcessTextMessage(QString message)
		{

		}

		void WebSocketThread::ProcessBinaryMessage(QByteArray message)
		{

		}

		void WebSocketThread::SocketDisconnected()
		{
			QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
			m_logger->debug("WebsocketServer started.");
			//qDebug() << "socketDisconnected:" << pClient->localAddress;
			if (pClient) {
				m_ClientList.removeAll(pClient);
				pClient->deleteLater();
			}
		}











		//ThreadedWebSocketServer::ThreadedWebSocketServer(const QUrl &url, bool debug) :
		//	m_url(url),
		//	m_debug(debug)
		//{
		//	connect(&m_webSocket, &QWebSocket::connected, this, &ThreadedWebSocketServer::onConnected);
		//	connect(&m_webSocket, &QWebSocket::disconnected, this, &ThreadedWebSocketServer::closed);
		//	m_webSocket.open(QUrl(QStringLiteral("ws://localhost:3010")));
		//}

		//void ThreadedWebSocketServer::onConnected()
		//{
		//	connect(&m_webSocket, &QWebSocket::textMessageReceived,
		//		this, &ThreadedWebSocketServer::onTextMessageReceived);
		//	m_webSocket.sendTextMessage(QStringLiteral("Hello, world!"));
		//}

		//void ThreadedWebSocketServer::onTextMessageReceived(QString message)
		//{
		//	AbstractCommand* cmd = nullptr;
		//	
		//	emit NewCommand(cmd);

		//	m_webSocket.close();
		//}
	}
}
