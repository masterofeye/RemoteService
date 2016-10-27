#include "CommunicationServer.h"
#include "qmetaobject.h"

#include "WebSocketServer.h"
#include "LocalServer.h"
#include "JobScheduler.h"

namespace RW{
	namespace CORE{
		CommunicationServer::CommunicationServer(QObject *Parent = nullptr) : BasicServer(Parent),
			m_logger(spdlog::get("file_logger")), 
			m_WebSocketServer(new WebSocketServer(this)),
			m_LocalServer(new LocalServer(this))
		{
		}


		CommunicationServer::~CommunicationServer()
		{
		}


		void CommunicationServer::PrepareIncomingConnection()
		{

		}

		bool CommunicationServer::Listen(quint16 Port)
		{
			if (m_WebSocketServer == nullptr)
			{
				m_logger->emerg("WebSocketServer isn't created");
				return false;
			}

			if (m_LocalServer == nullptr)
			{
				m_logger->emerg("LocalServer isn't created");
				return false;
			}

			if (!m_WebSocketServer->Listen(Port))
				return false;

			if(!m_LocalServer->Listen(Port + 1))
				return false;

			m_logger->debug("CommunicationServer listen is finished.");
			return true;
		}

		void CommunicationServer::Register(QObject* Receiver)
		{
			if (Receiver == nullptr)
			{
				m_logger->error("Can't register Receicer because he is null for Object ") << Receiver->objectName().toStdString();;
			}
			else
			{
				//Wir brauchen das MetaObjekt des Receivers um eine Liste der Methoden zu bekommen
				const QMetaObject* metaObject = Receiver->metaObject();
				if (metaObject == nullptr)
				{
					m_logger->error("Meta object is null for Object ") << Receiver->objectName().toStdString();
					return;
				}

				//Prüfen ob im Receiver Objekt die OnMessage Methode existiert
				int methodIndex = metaObject->indexOfMethod("OnMessage");
				//TODO MagicNumber
				if (methodIndex == -1)
				{
					m_logger->alert("There is no function called OnMessage for Object ") << Receiver->objectName().toStdString();;
					return;
				}

				QMetaMethod method = metaObject->method(methodIndex);

				const QMetaObject* metaObjectFromThis = this->metaObject();

				if (metaObjectFromThis == nullptr)
				{
					m_logger->error("Meta object is null for Object ") << Receiver->objectName().toStdString();
					return;
				}

				int signalIndex = metaObjectFromThis->indexOfSignal("Message");
				//TODO MagicNumber
				if (signalIndex == -1)
				{
					m_logger->alert("No Signal found with name NewCommand for Object ") << Receiver->objectName().toStdString();
					return;
				}

				QMetaMethod signal = metaObject->method(signalIndex);

				QMetaObject::Connection con = connect(this, signal, Receiver, method);
				if (((bool)con) == false)
				{
					m_logger->error("Connection couldn't established for Object: ") << Receiver->objectName().toStdString();
				}
				else
				{
					m_logger->debug("Receiver was successfully connected to the signal. ") << Receiver->objectName().toStdString();
				}
			}
		}

		void CommunicationServer::OnDisconnect()
		{
		}
		
		void CommunicationServer::OnSocketError(quint16 Error)
		{
		}

		void CommunicationServer::OnMessage(AbstractCommand* Command)
		{

		}

		void CommunicationServer::OnProcessedMessage(AbstractCommand* Command)
		{

		}

		void CommunicationServer::Unregister(QObject* Receiver)
		{
			
		}

		void CommunicationServer::UnregisterAll()
		{
		
		}
	}
}
