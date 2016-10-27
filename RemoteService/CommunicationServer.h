#pragma once
#include "qobject.h"

#include "BasicServer.h"

namespace RW{
	namespace CORE{

		class JobScheduler;
		class WebSocketServer;
		class LocalServer;

		class CommunicationServer :
			public BasicServer
		{
			Q_OBJECT
		private:
			WebSocketServer *m_WebSocketServer;
			LocalServer		*m_LocalServer;
			//TCPServer		m_TcpServer;
			std::shared_ptr<spdlog::logger> m_logger;
			QList<QObject*> m_ReceiverList;

		public:
			CommunicationServer(QObject *Parent);
			~CommunicationServer();

			void DisconectLater();

			void Register(QObject* Receiver);
			void Register(QObject* Receiver, CommandIds CommandId);
			void Unregister(QObject* Receiver);
			void UnregisterAll();

			virtual void PrepareIncomingConnection();
			virtual bool Listen(quint16 Port);
		public slots:
			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief
			@param Command
			@return void
			********************************************************************************************************************/
			virtual void OnDisconnect();

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief
			@param Command
			@return void
			********************************************************************************************************************/
			virtual void OnSocketError(quint16 Error);

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief
			@param Command
			@return void
			********************************************************************************************************************/
			virtual void OnMessage(AbstractCommand* Command);

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief
			@param Command
			@return void
			********************************************************************************************************************/
			virtual void OnProcessedMessage(AbstractCommand* Command);



		signals: 
			void Message(AbstractCommand* Command);
			void ProcessedMessage(AbstractCommand* Command);
			void SocketError(quint16 Error);
		};
	}
}
