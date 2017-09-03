#pragma once
#include "PowerStripeDevice.h"
#include <QUdpSocket>
#include "spdlog\spdlog.h"

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
	Q_OBJECT
private:
	QHostAddress m_IpAddress;
	QString m_Password = "anel";
	QString m_User = "admin";

	QUdpSocket *m_SendSocket;
	QUdpSocket *m_ReceiveSocket;

	Device m_Dev;
	Device_Socket* m_Sockets;
	std::shared_ptr<spdlog::logger> m_logger;
public:
    AnelHome(PeripheralType DeviceType, QVector<std::function<void(void)>> SwitchOnCondition, QObject *parent = 0);
	~AnelHome();

	bool Initialize();
	bool Reset();
    bool Deinitialize();

    virtual void Callback();
    virtual std::function<void(void)> GetCallback(TypeOfElement);

	virtual bool SwitchPort(quint8 Port, PortState St);
	virtual bool SwitchAll(PortState St);
	virtual bool GetPortState(quint8 Port, PortState &St);
private:
	void Switch(PortState Command, int Socket);
	void SwitchAll(PortState Command, int Sockets);
	void Parse(QByteArray Message);
	void SendAsk(QString Command);
private slots:
	void ReceiveAsk();
public slots:
    virtual void OnProcessMessage(COM::Message Msg);
private: 
	QString AnelHome::MapState(PortState Command);

};


	}

}