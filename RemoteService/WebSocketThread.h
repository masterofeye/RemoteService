#pragma once
#include "qobject.h"
#include <QtCore>

#include "spdlog\spdlog.h"

QT_BEGIN_NAMESPACE
class QWebSocket;
QT_END_NAMESPACE

//https ://www.youtube.com/watch?v=iKtCXUHsV70

namespace RW{
	namespace HW
	{
		class DeviceManager;
	}

	namespace CORE{
		class CommandBuilder;
		class AbstractCommand;
		
		class WebSocketThread : public QThread
		{
			Q_OBJECT
		private:
			std::shared_ptr<spdlog::logger> m_logger;
			QWebSocket *m_Socket;
			CommandBuilder *m_CmdBuilder;

		public:
			explicit WebSocketThread(QWebSocket *Socket, QObject *Parent = nullptr);
			~WebSocketThread();
			void run() Q_DECL_OVERRIDE;
		private slots:
			void ProcessTextMessage(QString message);
			void ProcessBinaryMessage(QByteArray message);
		public slots:

			void OnDisconnect();
			void OnProcessedMessage(AbstractCommand* Command);
			void OnSocketError(AbstractCommand *Command, quint16 Error);

		signals:
			void Message(AbstractCommand* Command);
			void SocketError(AbstractCommand* Command,int Error);
		};
	}
}