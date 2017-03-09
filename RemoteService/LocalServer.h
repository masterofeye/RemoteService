#pragma once
#include "BasicServer.h"
#include "spdlog\spdlog.h"

#include <QLocalSocket>

QT_BEGIN_NAMESPACE
class QLocalServer;
QT_END_NAMESPACE

namespace RW{


	namespace CORE{
		class LocalServer : public BasicServer
		{
			Q_OBJECT
#pragma region "Private Attributes"
		private:
			QLocalServer *m_Server;
			std::shared_ptr<spdlog::logger> m_logger;
            QMap<QString, QLocalSocket*>* m_SocketList;
            QLocalSocket* m_Socket;
#pragma endregion
#pragma region "Public Attributes"
		public:
#pragma endregion
#pragma region "Private Methods"
		private:
            static QByteArray Message(Util::Functions Func, QByteArray Message, Util::ErrorID Id);
            static QByteArray Message(Util::Functions Func, QByteArray Message);
#pragma endregion
#pragma region "Public Methods"
		public:
			LocalServer(QObject *Parent);
			~LocalServer(); 

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Alle eingehende Verbindungsversuche landen hier.
			@return void
			********************************************************************************************************************/
			virtual void PrepareIncomingConnection();

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Initialisiert den Server
			@param ChannelName
			@return Gibt zurück ob der Server nun auf den angebenen Port lauscht oder nicht.
			********************************************************************************************************************/
			virtual bool Listen(QString ChannelName);
#pragma endregion
#pragma region "Slots"
		public slots:
			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Wird aufgerufen sobald der LocalServer gestoppt werden soll.
			@return void
			********************************************************************************************************************/
			virtual void OnDisconnect();

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Diese Funktion wird aktuell nicht benötigt da sie durch die in Parameter abweichende OnSocketError 
			ersetzt wird.
			@param Error Gibt die Fehlercode an, warum vorhergehende Funktion eine Fehler hervorgerufen hat.
			@return void 
			********************************************************************************************************************/
			virtual void OnSocketError(quint16 Error);

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Wird aufgerufen sobald der LocalServer einen Fehlerstatus erreicht hat.
			@param Error Gibt den Fehlertype an.
			@return void
			********************************************************************************************************************/
			void OnSocketErrorNative(QLocalSocket::LocalSocketError Error);

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Diese Funktion wird vom Framework aufgerufen sobald ein Kommando erfolgreich abgearbeitet wurde.
			@param Command Enthält alle Informationen über das verarbeitete Kommando.
			@return void 
			********************************************************************************************************************/
            virtual void OnProcessMessage(Util::MessageReceiver Type, Util::Functions Func, QByteArray Data);

            void OnExternalMessage();
#pragma endregion
#pragma region "Signals"
		signals:

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Diese Signal wird ausgelöst, sobald ein neuer Befehl von den Clients an den Server gesendet wurde und die Informationen verarbeitet wurden.
			@param Command Kommando mit allen Information die nötig sind um die Verarbeitung zu starten.
			@return void
			********************************************************************************************************************/
            void NewMessage(Util::Functions MessageType, QByteArray Message);

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Diese Signal wird ausgelöst, sobald ein Kommand verarbeitet wurde und eine Antwort an eine oder mehrere Clients erfolgen soll.
			@remarks Dient nur als Erfüllung von Kompilererrors. Das Signal wird in dieser Klasse nicht benötigt. 
			@param Command Kommando mit allen Information die nötig sind um eine Antwort an einen oder mehrere Clients zu schicken
			@return void
			********************************************************************************************************************/
			void ProcessedMessage(AbstractCommand* Command);

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Diese Signal wird ausgelöst, sobald ein Kommand verarbeitet wurde und eine Antwort an eine oder mehrere Clients erfolgen soll.
			@param Command Kommando mit allen Information die nötig sind um eine Antwort an einen oder mehrere Clients zu schicken
			@return void
			********************************************************************************************************************/
			void SocketError(quint16 Error);

            /******************************************************************************************************************
            @autor Ivo Kunadt
            @brief Diese Signal wird ausgelöst, sobald der RemoteHiddenHelper eine Verbindung mit dem RemoteService hergestellt hat.
            @return void
            ********************************************************************************************************************/
            void RemoteHiddenHelperConnected();

#pragma endregion			
			
		};
	}
}