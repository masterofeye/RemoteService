#pragma once
#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		class USBDevice :
			public AbstractDevice
		{
		private: 
			/*!HardwareID des USB Ger�tes. Wird unteranderem um das Ger�te zur�cksetzen.*/
			QString m_HardwareID = "";
            QString m_Hub = "";
            QString m_Port = "";
		public:
            USBDevice(PeripheralType DeviceType, QVector<std::function<bool(void)>> SwitchOnCondition, QObject *parent = 0);
			virtual ~USBDevice();

			virtual bool Initialize();
			virtual bool Reset();
            virtual bool Deinitialize();
            virtual bool Callback(QString Pin, QString Port, QHostAddress IP);
            virtual std::function<bool(void)> GetCallback(TypeOfElement, QString, QString, QHostAddress);
		private:
			/*!
			@brief Parst den Output des Devcon Prozess und �berpr�ft ob ein Befehl richtig verarbeitet wurde.
			@param StdOutput Ein String mit dem Inhalt der R�ckgabe des Devcon Prozesses.
			@return Liefert true zur�ck, sofern das Kommando erfolgreich bearbeitet wurde.
			*/
			bool ParseStdOut(QString StdOutput);
         public slots:
            virtual void OnProcessMessage(COM::Message Msg);

		};
	}
}
