#pragma once
#include "PowerStripeDevice.h"
#include <QUdpSocket>

namespace RW{
	namespace HW{

const int PORT_SEND =  75;
const int PORT_RECEIVE = 77;


 struct Device
{
	QString Host_Name;
	QHostAddress IP;
	QHostAddress Mask;
	QHostAddress Gateway;
	QString MAC;
	QByteArray Blocked;
	ushort HTTP_Port;
	float Temperatur;
	QString FirmWare;
	char Typ;

	bool Power_Measurement;
	float VoltageRMS;
	float CurrentRMS;
	float LineFrequency;
	float ActivePower;
	float ApparentPower;
	float ReactivePower;
	float PowerFactor;
 };

 struct Device_Socket
 {
	 QString Socket_Name;
	 bool Socket_State;
	 QString IO_Name;
	 bool IO_Direction;
	 bool IO_State;
 };


class AnelHome :
	public PowerStripeDevice
{
private:
	QHostAddress m_IpAddress;
	QString m_Password;
	QString m_User;

	QUdpSocket *m_SendSocket;
	QUdpSocket *m_ReceiveSocket;

	Device m_Dev;
	Device_Socket* m_Sockets;
public:
	AnelHome(QObject *parent = 0);
	~AnelHome();

	bool Initialize();
	bool Reset();
	bool Shutdown();

	virtual bool SwitchPort(quint8 Port, State St);
	virtual bool SwitchAll(State St);
	virtual bool GetPortState(quint8 Port, State &St);
private:
	void Switch(State Command, int Socket);
	void SwitchAll(State Command, int Sockets);
	void Parse(QByteArray Message);
	void SendAsk(int SenderPort, QString Command);
	void ReceiveAsk();
private: 
	QString AnelHome::MapState(State Command);

};


	}

}