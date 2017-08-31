#pragma once
#include "AbstractDevice.h"

namespace RW{
	namespace HW{

		class USBDevice :
			public AbstractDevice
		{
		private: 
			/*!HardwareID des USB Gerätes. Wird unteranderem um das Geräte zurücksetzen.*/
			QString m_HardwareID;
		public:
            USBDevice(PeripheralType DeviceType, QObject *parent = 0);
			virtual ~USBDevice();

			virtual bool Initialize();
			virtual bool Reset();
			virtual bool Shutdown();
		private:
			/*!
			@brief Parst den Output des Devcon Prozess und überprüft ob ein Befehl richtig verarbeitet wurde.
			@param StdOutput Ein String mit dem Inhalt der Rückgabe des Devcon Prozesses.
			@return Liefert true zurück, sofern das Kommando erfolgreich bearbeitet wurde.
			*/
			bool ParseStdOut(QString StdOutput);
		};
	}
}
