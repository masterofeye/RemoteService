#pragma once
#include <QtCore>
#include <spdlog\spdlog.h>
#include <functional>

namespace RW{

    enum class PeripheralType;
	namespace HW{

		enum class PortState
		{
			ON,
			OFF,
			SNA
		};

        enum class DeviceConnectionType
        {
            USB,
            IO,
            RELAY,
            CAN

        };

        /*!
        @brief Enumeration, f�r den Zustand, die ein Ger�t einnehmen kann.
        */
        enum class State
        {
            Init,/*!< Ger�t ist initialisiert und Betriebsbereit*/
            Running,
            Deinit,/*!< Ger�t ist deinitialisiert*/
            Failure,/*!< Ger�t befinde sich im Fehlerzustand*/

        };



		class AbstractDevice : public QObject
		{
			Q_OBJECT
		private:
			QString m_DeviceName;
            PeripheralType m_DeviceType;
		protected:
			std::shared_ptr<spdlog::logger> m_Logger;


            State m_State = State::Failure;
		public:
            AbstractDevice(PeripheralType DeviceType, QObject *parent = 0);
			AbstractDevice(std::shared_ptr<spdlog::logger> Logger, QObject *parent = 0);
			~AbstractDevice();

			inline QString DeviceName(){ return m_DeviceName; }

			virtual bool Initialize() = 0;
			virtual bool Reset() = 0;
			virtual bool Shutdown() = 0;
		};
	}
}