#include "AnelHome.h"


namespace RW{
	namespace HW{

		AnelHome::AnelHome(QObject *parent) : PowerStripeDevice(parent),
			m_SendSocket(new QUdpSocket(this)),
			m_ReceiveSocket(new QUdpSocket(this)),
			m_logger(spdlog::get("remoteservice"))
		{
			m_Sockets = new Device_Socket[8];
			m_IpAddress.setAddress("192.168.0.24");
		}

		AnelHome::~AnelHome()
		{
			delete[] m_Sockets;
		}

		bool AnelHome::Initialize()
		{
			if (m_ReceiveSocket->bind(PORT_RECEIVE))
			{
				connect(m_ReceiveSocket, SIGNAL(readyRead()), this, SLOT(ReceiveAsk()));
				if (m_ReceiveSocket->open(QIODevice::OpenModeFlag::ReadOnly))
				{
					m_SendSocket->connectToHost(m_IpAddress, PORT_SEND);
				}
				else
				{
					m_logger->error("Can't open ReceiveSocket for AnelHome device","RemoteService");
					return false;
				}

			}
			else
			{
				m_logger->error("Can't bind to port {} on localhost", PORT_RECEIVE, "RemoteService");
				return false;
			}
			//qApp->processEvents();
			SendAsk("wer da?");
			//qApp->processEvents();
			return true;
		}


		QString AnelHome::MapState(PortState Command)
		{
			if (Command == PortState::ON)
				return "Sw_on";
			else if (Command == PortState::OFF)
				return "Sw_off";
			else
				return  "";
		}

		void AnelHome::Switch(PortState Command, int Socket)
		{
			if (!m_SendSocket->isOpen())
			{
				m_SendSocket->connectToHost(m_IpAddress, PORT_SEND);
				m_SendSocket->open(QIODevice::OpenModeFlag::WriteOnly);
			}
			m_SendSocket->open(QIODevice::OpenModeFlag::WriteOnly);
			QString cmd = MapState(Command) + QString::number(Socket) + m_User + m_Password;
			m_SendSocket->write(QByteArray(cmd.toUtf8()));
			m_SendSocket->close();
		}

		void AnelHome::SwitchAll(PortState Command, int Sockets)
		{
			if (!m_SendSocket->isOpen())
			{
				m_SendSocket->connectToHost(m_IpAddress, PORT_SEND);
				m_SendSocket->open(QIODevice::OpenModeFlag::WriteOnly);
			}
			QString cmd = MapState(Command) + QString::number(Sockets) + m_User + m_Password;
			m_SendSocket->write(QByteArray(cmd.toUtf8()));
			m_SendSocket->close();
		}


		void AnelHome::Parse(QByteArray Message)
		{
			QStringList splittedList = QString(Message).split(":");
			if (splittedList[0] != "NET-PwrCtrl") return;

			//Error wrong User;Pass
			if (splittedList[2] == "NoPass")
			{
				//NoPass = buf_split[1]; //NoPass=Hostname else NoPass=empty string
				return;
			}

			m_Dev.Host_Name = splittedList[1];
			m_Dev.IP = QHostAddress(splittedList[2]);
			m_Dev.Mask = QHostAddress(splittedList[3]);
			m_Dev.Gateway = QHostAddress(splittedList[4]);
			m_Dev.MAC = splittedList[5];

			for (int i = 0; i < 8; i++)
			{
				QStringList name_split = splittedList[6 + i].split(',');
				m_Sockets[i].Socket_Name = name_split[0];
				m_Sockets[i].Socket_State = name_split[1].toInt();
			}

			m_Dev.Blocked = splittedList[14].toUtf8();
			m_Dev.HTTP_Port = splittedList[15].toInt();
			m_Dev.Temperatur = -127;
		}

		void AnelHome::SendAsk(QString Command)
		{
			m_SendSocket->open(QIODevice::OpenModeFlag::WriteOnly);
			m_SendSocket->write(QByteArray(Command.toUtf8()));
			m_SendSocket->close();
		}

		void AnelHome::ReceiveAsk()
		{
			QByteArray datagram;
			while (m_ReceiveSocket->hasPendingDatagrams()) 
			{
				
				datagram.resize(m_ReceiveSocket->pendingDatagramSize());
				QHostAddress sender;
				quint16 senderPort;

				m_ReceiveSocket->readDatagram(datagram.data(), datagram.size(),
					&sender, &senderPort);
				Parse(datagram);
			}
			Parse(datagram);
		}

		/*
		@todo Need to be implemented
		*/
		bool AnelHome::Reset()
		{
			return true;
		}

		/*
		@todo Need to be implemented
		*/
		bool AnelHome::Shutdown()
		{
			return true;
		}

		bool AnelHome::SwitchPort(quint8 Port, PortState St)
		{
			Switch(St, Port);
			QThread::msleep(100);

            PortState check = PortState::SNA;
            GetPortState(Port, check);
            if (check == St)
                return true;
            else
                return false;

		}

		bool AnelHome::SwitchAll(PortState St)
		{
			SwitchAll(St,3);
			QThread::msleep(100);
			return true;
		}

		bool AnelHome::GetPortState(quint8 Port, PortState &St)
		{
			St = m_Sockets[Port].Socket_State == true ? PortState::ON : PortState::OFF;
			return true;
		}

	}
}