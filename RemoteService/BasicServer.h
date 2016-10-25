#pragma once
#include <QtCore>

#include "AbstractCommand.h"

namespace RW{
	namespace CORE{
		class BasicServer : public QObject
		{
			Q_OBJECT
		protected: 
			std::shared_ptr<spdlog::logger> m_logger;
		public:
			BasicServer(QObject *Parent);
			~BasicServer();

			virtual void PrepareIncomingConnection() = 0;
			virtual bool Listen(quint16 Port) = 0;
		public slots:
			virtual void OnDisconnect() = 0;
			virtual void OnSocketError(quint16 Error) = 0;
			virtual void OnMessage(AbstractCommand* Command) = 0;
			virtual void OnProcessedMessage(AbstractCommand* Command) = 0;
		signals:
			/*
			* @brief Will be emited after every incomming message that includes a command.
			*/
			void Message(AbstractCommand* Command);
			void ProcessedMessage(AbstractCommand* Command);
			void SocketError(quint16 Error);
		};

	}
}