#pragma once
#include <RemoteCommunicationLibrary.h>
#include <QtCore>
#include "spdlog\spdlog.h"



namespace RemoteBoxWrapper
{
	class Wrapper;
}


namespace RW{
	namespace HW
	{
		class RemoteBoxDevice;
		class PowerStripeDevice;
		class PowerSupplyDevice;
		class DeviceManager;
	}

	namespace CORE
	{
		enum class CommandIds
		{
			IO = 0, // Commands which manupulate IO's
			RELAY,
			USB,
			FHOST,
			ADC,
			DAC,
			UART,
			I2C,
			CFG, //Return the configuration of the current system
			POWERSTRIPE,
			POWERSUPPLY,
			FLASH,
			LOGOUT,
			SHUTDOWN,
			FAILURE
		};

		class AbstractCommand : public QObject
		{
			Q_OBJECT
				Q_ENUMS(ExecutionVariantType)
		public:
			enum class SourceAndDestination
			{
				RemoteView,
				RemoteApp, 
				RemoteService
			};
			Q_ENUM(SourceAndDestination);

			
			AbstractCommand *Next;
		protected:
			std::shared_ptr<spdlog::logger> m_logger;
			/*@brief Eindeutige ID des Kommandos*/
			COM::MessageDescription m_CommandID;
			/*@brief Quelle des Kommandos*/
			QString m_Src;
			/*@brief Ziel des Kommandos*/
			QString m_Dst;
			/*@brief Variante des Kommandos*/
            COM::Message::ExecutionVariant m_ExecutionVariant;
			/*@brief Liste an Parametern die vom Sender kommt*/
			QList<QVariant> m_ParameterList;
			/*@brief Rückgabewerte an den Sender*/
			QVariant m_Result;
			/*@brief Muss gesetzt werden bei der Ausführung des Kommands und wird an den Sender als Feedback geschickt.*/
			bool m_Success;
			/*@brief Gibt an ob das Parsen des JSON Anweisung erfolgreich verlaufen ist.*/
			bool m_Parsing;
		public:
            explicit AbstractCommand(const COM::Message Obj, QObject *Parent = nullptr);
            AbstractCommand(COM::MessageDescription CmdId, QString Source, QString Destination, COM::Message::ExecutionVariant ExecutionVariant, QList<QVariant> Parameter, QObject *Parent = nullptr);
			virtual ~AbstractCommand() {};

			virtual bool Execute(){ m_logger->debug("Command executed"); return true; }

			inline void SetSuccess(bool Success){ m_Success = Success; }
			inline void SetResult(QVariant Result){ m_Result = Result; }

            inline COM::MessageDescription CommandID(){ return m_CommandID; }
			inline QString Source(){ return m_Src; }
			inline QString Destionation(){ return m_Dst; }
			inline QList<QVariant> ParameterListe(){ return m_ParameterList; }
            inline COM::Message::ExecutionVariant ExecutionVariant(){ return m_ExecutionVariant; }
			inline QVariant Result(){ return m_Result; }
			inline bool Success(){ return m_Success; }
			inline bool IsParsed(){ return m_Parsing; }

			virtual void SetDevice(QObject* const Device = nullptr);

            inline COM::Message toMessage(){
                COM::Message m;
                m.SetMessageID(m_CommandID);
                m.SetParameterList(m_ParameterList);
                m.SetResult(m_Result);
                m.SetSuccess(m_Success);
                return m;
            }

		public slots: 
            void TransmitFinish(COM::Message Cmd);
		signals:
			void finished(COM::Message Cmd);
		};

		class RelayCommand : public AbstractCommand
		{
		private:
			RemoteBoxWrapper::Wrapper* m_RemoteBoxWrapper;
		public:
            explicit RelayCommand(const COM::Message Obj);
			virtual void SetDevice(QObject* const Device);
			virtual ~RelayCommand() {};
			virtual bool Execute();
		};

		class IOCommand : public AbstractCommand
		{
		private:
			RemoteBoxWrapper::Wrapper* m_RemoteBoxWrapper;
		public:
			explicit IOCommand(const COM::Message Obj);
			virtual void SetDevice(QObject* const Device);
			virtual ~IOCommand() {};
			virtual bool Execute();
		};

		class USBCommand : public AbstractCommand
		{
		private:
			RemoteBoxWrapper::Wrapper* m_RemoteBoxWrapper;
		public:
			explicit USBCommand(const COM::Message Obj);
			virtual void SetDevice(QObject* const Device);
			virtual ~USBCommand() {};
			virtual bool Execute();
		};

		class ADCCommand : public AbstractCommand
		{
		private:
			RemoteBoxWrapper::Wrapper* m_RemoteBoxWrapper;
		public:
			explicit ADCCommand(const COM::Message Obj);
			virtual void SetDevice(QObject* const Device);
			virtual ~ADCCommand() {};
			virtual bool Execute();
		};

		class DACCommand : public AbstractCommand
		{
		private:
			RemoteBoxWrapper::Wrapper* m_RemoteBoxWrapper;
		public:
			explicit DACCommand(const COM::Message Obj);
			virtual void SetDevice(QObject* const Device);
			virtual ~DACCommand() {};
			virtual bool Execute();
		};

		class PowerStripeCommand : public AbstractCommand
		{
		private:
			HW::PowerStripeDevice* m_Device;
		public:
			explicit PowerStripeCommand(const COM::Message Obj);
			virtual void SetDevice(QObject* const Device);
			virtual ~PowerStripeCommand() {};
			virtual bool Execute();
		};

		class PowerSupplyCommand : public AbstractCommand
		{
		private:
			HW::PowerSupplyDevice* m_Device;
		public:
			explicit PowerSupplyCommand(const COM::Message Obj);
			virtual void SetDevice(QObject* const  Device);
			virtual ~PowerSupplyCommand() {};
			virtual bool Execute();
		};

		class LogOutCommand : public AbstractCommand
		{
		public:
			explicit LogOutCommand(const COM::Message Obj);
			virtual void SetDevice(QObject* const Device);
			virtual ~LogOutCommand() {};
			virtual bool Execute();
		};

		class ShutdownCommand : public AbstractCommand
		{
		private: 
			HW::DeviceManager* m_Manager;
		public:
			explicit ShutdownCommand(const COM::Message Obj);
			virtual void SetDevice(QObject* const Device);
			virtual ~ShutdownCommand() {};
			virtual bool Execute();
		};

		class FlashCommand : public AbstractCommand
		{
			Q_OBJECT
			QThread m_workerThread;
		public:
			explicit FlashCommand(const COM::Message Obj);
			virtual void SetDevice(QObject* const Device);
			virtual ~FlashCommand() {};
			virtual bool Execute();
		signals:
			void Start(AbstractCommand* Cmd);
		};
	}
}