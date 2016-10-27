#pragma once


#include "BasicServer.h"
#include "spdlog\spdlog.h"
#include "WebSocketThread.h"
#include "AbstractCommand.h"

QT_BEGIN_NAMESPACE
class QWebSocketServer;
QT_END_NAMESPACE

namespace RW{
	namespace HW
	{
		class DeviceManager;
	}

	namespace CORE{
		class WebSocketServer : public BasicServer
		{
			Q_OBJECT
		private:
			QWebSocketServer* m_WebSocketServer;
			HW::DeviceManager *m_DeviceMng;
		public:
			WebSocketServer(QObject *Parent);
			~WebSocketServer();

			virtual void PrepareIncomingConnection();
			virtual bool Listen(quint16 Port);

		public slots:
			virtual void OnDisconnect();
			virtual void OnSocketError(quint16 Error);
			virtual void OnMessage(AbstractCommand* Command);
			virtual void OnProcessedMessage(AbstractCommand* Command);

		signals:
			void Message(AbstractCommand* Cmd);
			void ProcessedMessage(AbstractCommand* Cmd);
			void SocketError(quint16 Error);
		};

	}
}