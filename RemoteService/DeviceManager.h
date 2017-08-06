#pragma once
#include <QtCore>
#include "spdlog\spdlog.h"

namespace RW{
    namespace CORE
    {
        class ConfigurationManager;
    }
	namespace HW{

		class AbstractDevice;

        /*!
        @brief Der DeviceManager ist daf�r verantwortlich die ben�tigten HW Komponenten zu verwalten. Dies bedeutet sie zu initialisieren, 
        wenn der Rechner startet, Fehlerzust�nde zu erkennen und zu beheben und die HW wieder in den Ausgangszustand zuversetzen sobald ein Benutzer den 
        HW Arbeitsplatz verl�sst.
        */
		class DeviceManager : public QObject
		{
			Q_OBJECT
		public:
            /*!
            @brief Enumeration, welche eine Auflistung aller Ger�te, die �ber den DeviceManager gesteuert werden. 
            */
			enum class DeviceType
			{
				RemoteBox, /*!< enumeration f�r RemoteBox*/
				PowerStripe, /*!< enumeration f�r PowerStripe*/
				PowerSupply, /*!< enumeration f�r PowerSupply*/
			};

            /*!
            @brief Enumeration, f�r den Zustand, die ein Ger�t einnehmen kann.
            */
			enum class State
			{
				Init,/*!< Ger�t ist initialisiert und Betriebsbereit*/
				DeInit,/*!< Ger�t ist deinitialisiert*/
                Failure,/*!< Ger�t befinde sich im Fehlerzustand*/

			};
 
		private:
			std::shared_ptr<spdlog::logger> m_logger;
			State m_State;

			QMap<DeviceType,AbstractDevice*> *m_DeviceList;
            CORE::ConfigurationManager* m_ConfigManager;
		public:
            DeviceManager(CORE::ConfigurationManager* CfgManager, QObject *Parent = nullptr);
			virtual ~DeviceManager();

            /*!
            @brief Helferfunktion um die Loggerinstanz an den DeviceManager zu geben
            @param [in] Logger Loggerinstanz, diese wird durch die Hauptapplikation erstellt
            @return void
            */
			void DeviceManager::SetLogger(std::shared_ptr<spdlog::logger> Logger);

            /*!
            @brief Gibt eine Instanz auf ein Ger�te zur�ck. 
            Bevor diese Funktion genutzt werden kann, sollten die Ger�te initialisiert sein. 
            Eine Pr�fung auf null_ptr ist erforderlich.
            @param DeviceType Siehe daf�r \ref DeviceType 
            */
			AbstractDevice* GetDevice(DeviceType Type);

            /*!
            @brief Initialisiert alle Ger�te die unter der Kontrolle des DeviceManagers stehen.
            @return bool Sind alle Ger�te erfolgreich initialisert, liefert die Funktion true zur�ck.
            */
			bool Init();

            /*!
            @brief Deinitialisert alle Ger�te die unter der Kontrolle des DeviceManagers stehen. 
            @return bool Sind alle Ger�te erfolgreich deinitialisert, liefert die Funktion true zur�ck
            */
			bool DeInit();

            /*!
            @brief Liest alle HW die vom System erkannt wird ein.
            */
            bool CollectHardwarePeripherie();

            /*!
            @brief Registriert ein neues Ger�te beim DeviceManager. 
            @param DeviceName Name des Ger�tes, welches registriert werden soll.
            @return Gibt true zur�ck, wenn das Ger�te registriert werden konnte.
            */
            bool RegisterNewDevice(QString DeviceName);

            /*!
            @brief Deregistriert ein Ger�te vom DeviceManager. 
            @param DeviceName Name des Ger�tes, welches deregistriert werden soll.
            @return Gibt true zur�ck, wenn das Ger�te derigistriert werden konnte.
            */
            bool DeregisterNewDevice(QString DeviceName);


		};

	}
}