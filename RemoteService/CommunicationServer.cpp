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
            //Weiterleitung der Information an den Communicationserver
            connect(m_LocalServer, &LocalServer::RemoteHiddenHelperConnected, this, &CommunicationServer::OnRemoteHiddenHelperConnected);
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
                int clientMethodIndex = ClientMetaObject->indexOfSlot(QMetaObject::normalizedSignature("OnProcessMessage(Util::MessageReceiver, Util::Functions, QByteArray)"));
				//TODO MagicNumber
				if (clientMethodIndex == -1)
				{
					m_logger->warn("There is no function called OnProcessMessage for Object {}", Client->objectName().toStdString());
					return;
				}

				QMetaMethod clientOnProcessMessageMethod = ClientMetaObject->method(clientMethodIndex);

				const QMetaObject* ServerMetaObject = this->metaObject();
				if (ServerMetaObject == nullptr)
				{
					m_logger->error("Meta object is null for Object {}", this->objectName().toStdString());
					return;
				}

				int ServerSignalIndex = ServerMetaObject->indexOfSignal(QMetaObject::normalizedSignature("NewMessage(Util::MessageReceiver, Util::Functions, QByteArray)"));
				//TODO MagicNumber
				if (ServerSignalIndex == -1)
				{
					m_logger->warn("No Signal found with name NewCommand for Object {}", this->objectName().toStdString());
					return;
				}

				QMetaMethod serverSignal = ServerMetaObject->method(ServerSignalIndex);

				QMetaObject::Connection connectionServerClient = connect(this, serverSignal, Client, clientOnProcessMessageMethod);
                if (((bool)connectionServerClient) == false)
				{
					m_logger->error("Connection couldn't established for Object:{}", Client->objectName().toStdString());
				}
				else
				{
					m_logger->debug("Receiver was successfully connected to the signal. {}", Client->objectName().toStdString());
				}

                int serverMethodIndex = ServerMetaObject->indexOfSlot(QMetaObject::normalizedSignature("OnProcessMessage(Util::MessageReceiver, Util::Functions, QByteArray)"));
				if (serverMethodIndex == -1)
				{
					m_logger->warn("There is no function called OnProcessMessage for Object {}", this->objectName().toStdString());
					return;
				}

				QMetaMethod serverOnProcessMessageMethod = ServerMetaObject->method(serverMethodIndex);

                int clientSignalIndex = ClientMetaObject->indexOfSignal(QMetaObject::normalizedSignature("NewMessage(Util::MessageReceiver, Util::Functions, QByteArray)"));
				//TODO MagicNumber
				if (clientSignalIndex == -1)
				{
					m_logger->warn("No Signal found with name NewCommand for Object {}", this->objectName().toStdString());
					return;
				}

                QMetaMethod clientSignal = ClientMetaObject->method(clientSignalIndex);

				QMetaObject::Connection connectionClientServer = connect(Client, clientSignal, this, serverOnProcessMessageMethod);
                if (((bool)connectionClientServer) == false)
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
            Q_UNUSED(Error);
		}


        void CommunicationServer::OnProcessMessage(Util::MessageReceiver Type, Util::Functions Func, QByteArray Data)
		{
            switch (Type)
            {
            case RW::CORE::Util::MessageReceiver::CommunicationServer:
            case RW::CORE::Util::MessageReceiver::ProcessManager:
            case RW::CORE::Util::MessageReceiver::CanEasyWrapper:
            case RW::CORE::Util::MessageReceiver::BasicWrapper:
            case RW::CORE::Util::MessageReceiver::PortalInfo:
            case RW::CORE::Util::MessageReceiver::FHostSPWrapper:
            case RW::CORE::Util::MessageReceiver::MKSWrapper:
            case RW::CORE::Util::MessageReceiver::FileUtil:
            case RW::CORE::Util::MessageReceiver::UsbHidLoader:
            case RW::CORE::Util::MessageReceiver::ShutdownHandler:
            case RW::CORE::Util::MessageReceiver::InactivityWatcher:
                m_LocalServer->OnProcessMessage(Type, Func, Data);
                break;
            default:
                break;
            }
		}

		void CommunicationServer::Unregister(QObject* Receiver)
		{
            disconnect(Receiver);
		}

		void CommunicationServer::UnregisterAll()
		{
		   
		}

        void CommunicationServer::OnRemoteHiddenHelperConnected()
        {
            emit RemoteHiddenHelperConnected();
        }

	}
}
