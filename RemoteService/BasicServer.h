#pragma once
#include <QtCore>

#include "Constants.h"
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
		public slots:
			virtual void OnDisconnect() = 0;
			virtual void OnSocketError(quint16 Error) = 0;
            virtual void OnProcessMessage(Util::MessageReceiver Type, Util::Functions Func, QByteArray Data) = 0;
		signals:
			/*
			* @brief Will be emited after every incomming message that includes a command.
			*/
            void NewMessage(Util::Functions MessageType, QByteArray Message);
			void ProcessedMessage(AbstractCommand* Command);
			void SocketError(quint16 Error);
		};

	}
}