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
        @brief Der DeviceManager ist dafür verantwortlich die benötigten HW Komponenten zu verwalten. Dies bedeutet sie zu initialisieren, 
        wenn der Rechner startet, Fehlerzustände zu erkennen und zu beheben und die HW wieder in den Ausgangszustand zuversetzen sobald ein Benutzer den 
        HW Arbeitsplatz verlässt.
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
            @brief Wird intern verwendet um Geräte zu registrieren
            @param [in] Device
            @return True, sofern die Registrierung des Gerätes erfolgreich verlaufen ist.
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
            @brief Gibt eine Instanz auf ein Geräte zurück. 
            Bevor diese Funktion genutzt werden kann, sollten die Geräte initialisiert sein. 
            Eine Prüfung auf null_ptr ist erforderlich.
            @param DeviceType Siehe dafür \ref DeviceType 
            */
            AbstractDevice* GetDevice(PeripheralType Type);

            /*!
            @brief Initialisiert alle Geräte die unter der Kontrolle des DeviceManagers stehen.
            @return bool Sind alle Geräte erfolgreich initialisert, liefert die Funktion true zurück.
            */
			bool Init();

            /*!
            @brief Deinitialisert alle Geräte die unter der Kontrolle des DeviceManagers stehen. 
            @return bool Sind alle Geräte erfolgreich deinitialisert, liefert die Funktion true zurück
            */
			bool DeInit();

            /*!
            @brief Registriert ein neues Geräte beim DeviceManager. 
            @param DeviceName Name des Gerätes, welches registriert werden soll.
            @return Gibt true zurück, wenn das Geräte registriert werden konnte.
            */
            bool RegisterNewDevice(QString DeviceName);



            /*!
            @brief Deregistriert ein Geräte vom DeviceManager. 
            @param DeviceName Name des Gerätes, welches deregistriert werden soll.
            @return Gibt true zurück, wenn das Geräte derigistriert werden konnte.
            */
            bool DeregisterNewDevice(QString DeviceName);


		};

	}
}