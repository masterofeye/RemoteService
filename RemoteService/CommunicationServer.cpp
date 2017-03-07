#include "CommunicationServer.h"
#include <qtcpsocket.h>

#include "qmetaobject.h"
#include "LocalServer.h"
#include "JobScheduler.h"
#include "TCPServer.h"



namespace RW{
	namespace CORE{
		CommunicationServer::CommunicationServer(QObject *Parent = nullptr) : BasicServer(Parent),
			m_logger(spdlog::get("file_logger")), 
			m_LocalServer(new LocalServer(this)),
			m_TcpServer(new TCPServer(this))
		{
		}


		CommunicationServer::~CommunicationServer()
		{
		}


		void CommunicationServer::PrepareIncomingConnection()
		{
			//QTcpSocket* socket = m_TcpServer->nextPendingConnection();
			//if (socket != nullptr)
			//{
			//	connect(socket, &socket->readyRead,this, 

			//	QByteArray command = socket->readAll();
			//	if (QString(command).contains("UserInfo"))
			//	{
			//		socket->write("UserInfo");

			//		char username[UNLEN + 1];
			//		DWORD username_len = UNLEN + 1;
			//		GetUserNameA(username, &username_len);

			//		QByteArray answer;
			//		answer.append(username + ',');
			//		answer.append("false" + ',');

			//		char computername[UNLEN + 1];
			//		DWORD computernane_len = UNLEN + 1;
			//		GetComputerNameA(computername, &computernane_len);

			//		answer.append(computername + ',');
			//		answer.append("192.156.244.111" + ',');
			//		answer.append("192.156.244.111" + ',');
			//		answer.append("" + ',');
			//		answer.append("Streamlist{}");
			//	}
			//}
		}

		//void CommunicationServer::ReadCommand()
		//{
		//	QDataStream in;
		//	in.setDevice(tcpSocket);
		//	in.setVersion(QDataStream::Qt_4_0);
		//	in.startTransaction();
		//}


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
			Sleep(100);
			qApp->processEvents();

			if (m_LocalServer == nullptr)
			{
				m_logger->error("LocalServer isn't created");
				return false;
			}

			if(!m_LocalServer->Listen("Server"))
			{
				m_logger->error("LocalServer couldn't list on port: {}", Port + 1);
				return false;
			}

			m_logger->debug("CommunicationServer is ready.");
#ifdef DEBUG
			m_logger->flush();
#endif // DEBUG

			return true;
		}

		void CommunicationServer::Register(QObject* Client)
		{
			if (Client == nullptr)
			{
				m_logger->error("Can't register Client because he is null for Object {}", Client->objectName().toStdString());
			}
			else
			{
				//Wir brauchen das MetaObjekt des Receivers um eine Liste der Methoden zu bekommen
				const QMetaObject* ClientMetaObject = Client->metaObject();
				if (ClientMetaObject == nullptr)
				{
					m_logger->error("Meta object is null for Object {}", Client->objectName().toStdString());
					return;
				}

				//Prüfen ob im Receiver Objekt die OnMessage Methode existiert
				int clientMethodIndex = ClientMetaObject->indexOfMethod("OnProcessMessage");
				//TODO MagicNumber
				if (clientMethodIndex == -1)
				{
					m_logger->warn("There is no function called OnMessage for Object {}", Client->objectName().toStdString());
					return;
				}

				QMetaMethod clientOnProcessMessageMethod = ClientMetaObject->method(clientMethodIndex);

				const QMetaObject* ServerMetaObject = this->metaObject();

				if (ServerMetaObject == nullptr)
				{
					m_logger->error("Meta object is null for Object {}", this->objectName().toStdString());
					return;
				}

				int ServerSignalIndex = ServerMetaObject->indexOfSignal("NewMessage");
				//TODO MagicNumber
				if (ServerSignalIndex == -1)
				{
					m_logger->warn("No Signal found with name NewCommand for Object {}", this->objectName().toStdString());
					return;
				}

				QMetaMethod serverSignal = ServerMetaObject->method(ServerSignalIndex);

				QMetaObject::Connection con = connect(this, serverSignal, Client, clientOnProcessMessageMethod);
				if (((bool)con) == false)
				{
					m_logger->error("Connection couldn't established for Object:{}", Client->objectName().toStdString());
				}
				else
				{
					m_logger->debug("Receiver was successfully connected to the signal. {}", Client->objectName().toStdString());
				}

				int serverMethodIndex = ServerMetaObject->indexOfMethod("OnProcessMessage");
				if (serverMethodIndex == -1)
				{
					m_logger->warn("There is no function called OnMessage for Object {}", this->objectName().toStdString());
					return;
				}

				QMetaMethod serverOnProcessMessageMethod = ServerMetaObject->method(serverMethodIndex);

				int clientSignalIndex = ServerMetaObject->indexOfSignal("NewMessage");
				//TODO MagicNumber
				if (clientSignalIndex == -1)
				{
					m_logger->warn("No Signal found with name NewCommand for Object {}", this->objectName().toStdString());
					return;
				}

				QMetaMethod clientSignal = ServerMetaObject->method(clientSignalIndex);

				QMetaObject::Connection con = connect(Client, clientSignal, this, serverOnProcessMessageMethod);
				if (((bool)con) == false)
				{
					m_logger->error("Connection couldn't established for Object:{}", this->objectName().toStdString());
				}
				else
				{
					m_logger->debug("Receiver was successfully connected to the signal. {}", this->objectName().toStdString());
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

		QDataStream &operator <<(QDataStream &out, const RW::CORE::Message &dataStruct)
		{
			out.startTransaction();
			out << (quint16)dataStruct.MessageType;
			out << dataStruct.MessageSize;
			out.writeRawData(dataStruct.Message, dataStruct.MessageSize);
			out << (quint16)dataStruct.Error;
			out.commitTransaction();
			return out;
		}

		QDataStream &operator >>(QDataStream &in, RW::CORE::Message &dataStruct)
		{
			quint16 messageType = 0;
			quint16 id = 0;
			in >> messageType;
			in >> dataStruct.MessageSize;
			dataStruct.Message.resize(dataStruct.MessageSize);
			in.readRawData(dataStruct.Message.data(), dataStruct.MessageSize);
			in >> id;

			dataStruct.MessageType = static_cast<RW::CORE::Util::Functions>(messageType);
			dataStruct.Error = static_cast<RW::CORE::Util::ErrorID>(id);
			return in;
		}

		QByteArray CommunicationServer::Message(Util::Functions Func, QByteArray Message, Util::ErrorID Id)
		{
			QByteArray arr;
			RW::CORE::Message m(Func, Message.size(), Message, Id);
			QDataStream in(&arr, QIODevice::WriteOnly);
			in.setVersion(QDataStream::Qt_5_7);
			in << m;
			return arr;
		}
	}
}
