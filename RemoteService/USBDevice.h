#pragma once
#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		class USBDevice :
			public AbstractDevice
		{
		private: 
			/*!HardwareID des USB Ger�tes. Wird unteranderem um das Ger�te zur�cksetzen.*/
			QString m_HardwareID;
		public:
            USBDevice(PeripheralType DeviceType, QObject *parent = 0);
			virtual ~USBDevice();

			virtual bool Initialize();
			virtual bool Reset();
			virtual bool Shutdown();
		private:
			/*!
			@brief Parst den Output des Devcon Prozess und �berpr�ft ob ein Befehl richtig verarbeitet wurde.
			@param StdOutput Ein String mit dem Inhalt der R�ckgabe des Devcon Prozesses.
			@return Liefert true zur�ck, sofern das Kommando erfolgreich bearbeitet wurde.
			*/
			bool ParseStdOut(QString StdOutput);
		};
	}
}
