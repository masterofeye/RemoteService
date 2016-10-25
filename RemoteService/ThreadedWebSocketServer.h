#pragma once
#include <qthread.h>

#include "spdlog\spdlog.h"


QT_BEGIN_NAMESPACE
class QWebSocketServer;
class QWebSocket;
QT_END_NAMESPACE

namespace RW
{
	namespace CORE{

		class AbstractCommand;

		class WebSocketThread : public QThread
		{
			Q_OBJECT
		private:
			QWebSocketServer *m_WebSocketServer;
			QList<QWebSocket* const> m_ClientList;
			std::shared_ptr<spdlog::logger> m_logger;
		public:
			explicit WebSocketThread(QObject *Parent);

		public slots:
			void StartServer(quint16 Port);
			void run();

		private:

		};




		
	}
}