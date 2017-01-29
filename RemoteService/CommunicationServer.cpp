#include "CommunicationServer.h"
#include <qtcpsocket.h>

#include "qmetaobject.h"
#include "LocalServer.h"
#include "JobScheduler.h"


#include <Lmcons.h>

namespace RW{
	namespace CORE{
		CommunicationServer::CommunicationServer(QObject *Parent = nullptr) : BasicServer(Parent),
			m_logger(spdlog::get("file_logger")), 
			m_LocalServer(new LocalServer(this)),
			m_TcpServer(new QTcpServer(this))
		{
		}


		CommunicationServer::~CommunicationServer()
		{
		}


		void CommunicationServer::PrepareIncomingConnection()
		{
			QTcpSocket* socket = m_TcpServer->nextPendingConnection();
			if (socket != nullptr)
			{
				QByteArray command = socket->readAll();
				if (QString(command).contains("UserInfo"))
				{
					socket->write("UserInfo");

					char username[UNLEN + 1];
					DWORD username_len = UNLEN + 1;
					GetUserNameA(username, &username_len);

					QByteArray answer;
					answer.append(username + ',');
					answer.append("false" + ',');

					char computername[UNLEN + 1];
					DWORD username_len = UNLEN + 1;
					GetComputerNameA(computername, &username_len);

					answer.append(computername + ',');
					answer.append("192.156.244.111" + ',');
					answer.append("192.156.244.111" + ',');
					answer.append("" + ',');
					answer.append("Streamlist{}");
				}

				

			}
		}

		bool CommunicationServer::Listen(quint16 Port)
		{
			if (m_TcpServer == nullptr)
			{
				m_logger->error("TCP Server isn't created");
				return false;
			}

			if (!m_TcpServer->listen(QHostAddress::Any, Port))
			{
				m_logger->error("Tcp Server couldn't list on port: ");
				return false;
			}

			connect(m_TcpServer, &QTcpServer::newConnection, this, &CommunicationServer::PrepareIncomingConnection);

			if (m_LocalServer == nullptr)
			{
				m_logger->error("LocalServer isn't created");
				return false;
			}

			if(!m_LocalServer->Listen(Port+1))
			{
				m_logger->error("LocalServer couldn't list on port: ");
				return false;
			}

			m_logger->debug("CommunicationServer listen is finished.");
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG

			return true;
		}

		void CommunicationServer::Register(QObject* Receiver)
		{
			if (Receiver == nullptr)
			{
				m_logger->error("Can't register Receicer because he is null for Object "); //<< Receiver->objectName().toStdString();;
			}
			else
			{
				//Wir brauchen das MetaObjekt des Receivers um eine Liste der Methoden zu bekommen
				const QMetaObject* metaObject = Receiver->metaObject();
				if (metaObject == nullptr)
				{
					m_logger->error("Meta object is null for Object "); //<< Receiver->objectName().toStdString();
					return;
				}

				//Prüfen ob im Receiver Objekt die OnMessage Methode existiert
				int methodIndex = metaObject->indexOfMethod("OnMessage");
				//TODO MagicNumber
				if (methodIndex == -1)
				{
					m_logger->warn("There is no function called OnMessage for Object ");// << Receiver->objectName().toStdString();;
					return;
				}

				QMetaMethod method = metaObject->method(methodIndex);

				const QMetaObject* metaObjectFromThis = this->metaObject();

				if (metaObjectFromThis == nullptr)
				{
					m_logger->error("Meta object is null for Object ");//<< Receiver->objectName().toStdString();
					return;
				}

				int signalIndex = metaObjectFromThis->indexOfSignal("Message");
				//TODO MagicNumber
				if (signalIndex == -1)
				{
					m_logger->warn("No Signal found with name NewCommand for Object ");// << Receiver->objectName().toStdString();
					return;
				}

				QMetaMethod signal = metaObject->method(signalIndex);

				QMetaObject::Connection con = connect(this, signal, Receiver, method);
				if (((bool)con) == false)
				{
					m_logger->error("Connection couldn't established for Object: "); //  << Receiver->objectName().toStdString();
				}
				else
				{
					m_logger->debug("Receiver was successfully connected to the signal. ");// << Receiver->objectName().toStdString();
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
