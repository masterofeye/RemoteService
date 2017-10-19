#pragma once
#include <QtCore>
#include <qhostaddress.h>
#include <functional>
#include <spdlog\spdlog.h>
#include "RemoteCommunicationLibrary.h"

namespace RW{
    enum class TypeOfElement;
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
        class AbstractDevice;




		class AbstractDevice : public QObject
		{
			Q_OBJECT
		private:
			QString m_DeviceName;
            PeripheralType m_DeviceType;

		protected:
            std::shared_ptr<spdlog::logger> m_Logger;
            State m_State = State::Failure;
            QVector<std::function<bool(void)>> m_SwitchOnCondition;
        public slots:
            virtual void OnProcessMessage(COM::Message Msg) = 0;
        signals:
            void NewMessage(COM::Message Msg);
		public:
            AbstractDevice(PeripheralType DeviceType, QVector<std::function<bool(void)>> SwitchOnCondition, QObject *parent = 0);
			AbstractDevice(std::shared_ptr<spdlog::logger> Logger, QObject *parent = 0);
			~AbstractDevice();

			inline QString DeviceName(){ return m_DeviceName; }

            /*
            @brief Initialisiert das Ger�te und f�hrt alle notwendigen Schritte f�r die Inbetriebnahme aus. Dies beinhaltet auch, 
            dass das manipulieren von anderen Ger�ten.
            @return Liefer true zur�ck, sofern das Ger�t initialisiert ist.
            */
			virtual bool Initialize() = 0;
			virtual bool Reset() = 0;
            virtual bool Deinitialize() = 0;
            virtual bool Callback(QString Pin, QString Port, QHostAddress IP) = 0;
            virtual std::function<bool(void)> GetCallback(TypeOfElement, QString, QString, QHostAddress) = 0;

		};
	}
}