#include "LocalServer.h"

#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qdatastream.h>
#include "Constants.h"

namespace RW{
	namespace CORE{

		LocalServer::LocalServer(QObject *Parent = nullptr) : BasicServer(Parent),
			m_logger(spdlog::get("file_logger"))
		{
			m_Server = new QLocalServer(this);
            m_SocketList = new QMap<QString, QLocalSocket*>();

			//Jeder hat Zugriff auf den Server.
			m_Server->setSocketOptions(QLocalServer::WorldAccessOption);
			//Der Server akzeptiert nur eine Verbindung, das ist der RemoteService
			m_Server->setMaxPendingConnections(1);

			connect(m_Server, &QLocalServer::newConnection, this, &LocalServer::PrepareIncomingConnection);
		}

		LocalServer::~LocalServer()
		{
			m_Server->close();
			delete m_SocketList;
		}

        QDataStream &operator <<(QDataStream &out, const RW::CORE::Message &dataStruct)
        {
            out.startTransaction();
            out << (quint16)dataStruct.MessageFunc;
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

            dataStruct.MessageFunc = static_cast<RW::CORE::Util::Functions>(messageType);
            dataStruct.Error = static_cast<RW::CORE::Util::ErrorID>(id);
            return in;
        }

        QByteArray LocalServer::Message(Util::Functions Func, QByteArray Message, Util::ErrorID Id)
        {
            QByteArray arr;
            RW::CORE::Message m(Func, Message.size(), Message, Id);
            QDataStream in(&arr, QIODevice::WriteOnly);
            in.setVersion(QDataStream::Qt_5_7);
            in << m;
            return arr;
        }

        QByteArray LocalServer::Message(Util::Functions Func, QByteArray Message)
        {
            QByteArray arr;
            RW::CORE::Message m(Func, Message.size(), Message, Util::ErrorID::Success);
            QDataStream in(&arr, QIODevice::WriteOnly);
            in.setVersion(QDataStream::Qt_5_7);
            in << m;
            return arr;
        }

		void LocalServer::PrepareIncomingConnection()
		{
            m_Socket = m_Server->nextPendingConnection();

            if (m_Socket->isValid())
			{
				m_logger->debug("New Client is connected to the RemoteService");
                //m_SocketList->insert(m_Socket, "RemoteHiddenHelper");

                connect(m_Socket, SIGNAL(disconnected()), this, SLOT(OnSocketDisconnected()));
                connect(m_Socket, SIGNAL(readyRead()), this, SLOT(OnExternalMessage()));
                connect(m_Socket, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(OnSocketErrorNative(QLocalSocket::LocalSocketError)));
			}
		}

		bool LocalServer::Listen(QString ChannelName)
		{
			if (m_Server->listen(ChannelName))
			{
				m_logger->debug("LocalServer is started on ChannelName {}", ChannelName.toStdString());
				return true;
			}
			else
			{
				m_logger->error("Can't start local server on ChannelName {}", ChannelName.toStdString());
				return false;
			}
		}
 

		void LocalServer::OnDisconnect()
		{
			//QLocalSocket* LocalSocket = (QLocalSocket *)sender();
			//QString clientname = m_SocketList->find(LocalSocket).value();
			//m_logger->debug("Client disonnected from LocalServer: {}", clientname.toStdString());
		}

        void LocalServer::OnProcessMessage(Util::MessageReceiver Type, Util::Functions Func, QByteArray Data)
		{
            Q_UNUSED(Type);

            switch (Func)
            {
            case RW::CORE::Util::Functions::CanEasyStartApplication:
                break;
            case RW::CORE::Util::Functions::CanEasyLoadWorkspace:
                break;
            case RW::CORE::Util::Functions::CanEasyCloseApplication:
                break;
            case RW::CORE::Util::Functions::CanEasyStartSimulation:
                break;
            case RW::CORE::Util::Functions::CanEasyStopSimulation:
                break;
            case RW::CORE::Util::Functions::MKSLogin:
                break;
            case RW::CORE::Util::Functions::MKSStartDownload:
                break;
            case RW::CORE::Util::Functions::MKSCreateSandBox:
                break;
            case RW::CORE::Util::Functions::MKSDropSandbox:
                break;
            case RW::CORE::Util::Functions::MKSClose:
                break;
            case RW::CORE::Util::Functions::FHostSPStartFHost:
                break;
            case RW::CORE::Util::Functions::FHostSPLoadFlashFile:
                break;
            case RW::CORE::Util::Functions::FHostSPCloseFHost:
                break;
            case RW::CORE::Util::Functions::FHostSPGetState:
                break;
            case RW::CORE::Util::Functions::FHostSPAbortSequence:
                break;
            case RW::CORE::Util::Functions::FHostSPCloseSequence:
                break;
            case RW::CORE::Util::Functions::FHostSPStartFlash:
                break;
            case RW::CORE::Util::Functions::FHostSPGetProgress:
                break;
            case RW::CORE::Util::Functions::PortalInfoFittingAC:
                break;
            case RW::CORE::Util::Functions::PortalInfoAcByIcom:
                break;
            case RW::CORE::Util::Functions::PortalInfoReleases:
                break;
            case RW::CORE::Util::Functions::PortalInfoSoftwareById:
                break;
            case RW::CORE::Util::Functions::PortalInfoShowDialog:
                break;
            case RW::CORE::Util::Functions::PortalInfoCloseDialog:
                break;
            case RW::CORE::Util::Functions::FileUtilUnZip:
                break;
            case RW::CORE::Util::Functions::FileUtilDelete:
                break;
            case RW::CORE::Util::Functions::UsbHidLoaderFlashFile:
                break;
            case RW::CORE::Util::Functions::PrintDebugInformation:
                break;
            case RW::CORE::Util::Functions::ToggleCl30Fast:
                break;
            case RW::CORE::Util::Functions::ToggleCl30Slow:
                break;
            case RW::CORE::Util::Functions::StartShutdownTimer:
                break;
            case RW::CORE::Util::Functions::StopShutdownTimer:
                break;
            case RW::CORE::Util::Functions::StartInactivityTimer:
                break;
            case RW::CORE::Util::Functions::StopInactivityTimer:
                break;
            case RW::CORE::Util::Functions::Amount:
                break;
            default:
                break;
            }

            QByteArray message = Message(Func, Data);
            m_Socket->write(message);
            m_Socket->flush();
		}

        void LocalServer::OnExternalMessage()
        {
            QLocalSocket* socket = qobject_cast<QLocalSocket*>(QObject::sender());

            quint64 bytes = socket->bytesAvailable();
            if (bytes > 0)
            {
                QByteArray message = socket->readAll();

                //Jeder Client muss sich in der ersten Nachricht identifizieren. Somit ist sichergestellt, das Nachrichten später 
                //an den richtigen Client geschickt werden können.
                if (QString(message).contains("identifier"))
                {
                    QString identifier = QString(message).section("=", 1, 1);
                    //Es ist nicht nötif zu überprüfen, ob Clients schon vorhanden sind, da sie in der "OnDisconnect" Methode
                    //aus der Liste der Sockets gelöscht werden
                    m_SocketList->insert(identifier, socket);
                    //Handelt es sich um den RemoteHiddenHelper muss das System darüber informiert werden, um z.b. die Userüberwachung zu aktivieren
                    if (identifier.contains("remotehiddenhelper"))
                    {
                        emit RemoteHiddenHelperConnected();
                        m_logger->debug("RemoteHiddenHelper is connected");
                    }
                }
                else
                {
                    RW::CORE::Message m;
                    QDataStream dataStream(socket);
                    dataStream >> m;
                    emit NewMessage(m.MessageFunc, m.Message);
                }
            }
        }


		void LocalServer::OnSocketError(quint16 Error)
		{
			emit SocketError(Error);
		}

		void LocalServer::OnSocketErrorNative(QLocalSocket::LocalSocketError Error)
		{
			switch (Error)
			{
			case QLocalSocket::ConnectionRefusedError:
				m_logger->error("A LocalServer socket error occoured: {}", "ConnectionRefusedError");
				break;
			case QLocalSocket::PeerClosedError:
				m_logger->error("A LocalServer socket error occoured: {}", "PeerClosedError");
				break;
			case QLocalSocket::ServerNotFoundError:
				m_logger->error("A LocalServer socket error occoured: {}", "ServerNotFoundError");
				break;
			case QLocalSocket::SocketAccessError:
				m_logger->error("A LocalServer socket error occoured: {}", "SocketAccessError");
				break;
			case QLocalSocket::SocketResourceError:
				m_logger->error("A LocalServer socket error occoured:  {}", "SocketResourceError"); 
				break;
			case QLocalSocket::SocketTimeoutError:
				m_logger->error("A LocalServer socket error occoured:  {}", "SocketTimeoutError");
				break;
			case QLocalSocket::DatagramTooLargeError:
				m_logger->error("A LocalServer socket error occoured:  {}", "DatagramTooLargeError");
				break;
			case QLocalSocket::ConnectionError:
				m_logger->error("A LocalServer socket error occoured: {}","ConnectionError"); 
				break;
			case QLocalSocket::UnsupportedSocketOperationError:
				m_logger->error("A LocalServer socket error occoured:  {}", "UnsupportedSocketOperationError");
				break;
			case QLocalSocket::UnknownSocketError:
				m_logger->error("A LocalServer socket error occoured:  {}", "UnknownSocketError");
				break;
			case QLocalSocket::OperationError:
				m_logger->error("A LocalServer socket error occoured:  {}", "OperationError");
				break;
			default:
				break;
			}

			OnSocketError(0);
		}
	}
}