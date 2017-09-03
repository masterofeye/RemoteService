#pragma once
#include <QtCore>
#include "spdlog\spdlog.h"

namespace RW{
    enum class PeripheralType;

    namespace CORE
    {
        class ConfigurationManager;
        class WinApiDeviceHelper;
    }

    namespace SQL
    {
        class Peripheral;
    }

	namespace HW{
        enum class State;
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

 
		private:
			std::shared_ptr<spdlog::logger> m_logger;
			State m_State;

            QMap<PeripheralType, AbstractDevice*> *m_DeviceList;
            CORE::ConfigurationManager* m_ConfigManager;
            CORE::WinApiDeviceHelper* m_WinHelper;
        private:

            /*!
            @brief Wird intern verwendet um Ger�te zu registrieren
            @param [in] Device
            @return True, sofern die Registrierung des Ger�tes erfolgreich verlaufen ist.
            */
            bool RegisterNewDevice(RW::SQL::Peripheral Device);

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
            AbstractDevice* GetDevice(PeripheralType Type);

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