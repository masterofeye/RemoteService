#include "WebSocketThread.h"
#include <QtWebSockets/QWebSocket>

#include "AbstractCommand.h"
#include "CommandBuilder.h"

namespace RW{
	namespace CORE{
		

		WebSocketThread::WebSocketThread(QWebSocket *Socket, QObject *Parent) : QThread(Parent),
			m_Socket(Socket),
			m_logger(spdlog::get("file_logger")),
			m_CmdBuilder(new CommandBuilder())
		{
		}

		WebSocketThread::~WebSocketThread()
		{
			delete m_CmdBuilder;
		}


		void WebSocketThread::run()
		{
			connect(m_Socket, &QWebSocket::textMessageReceived, this, &WebSocketThread::ProcessTextMessage);
			connect(m_Socket, &QWebSocket::binaryMessageReceived, this, &WebSocketThread::ProcessBinaryMessage);
			connect(m_Socket, &QWebSocket::disconnected, this, &WebSocketThread::OnDisconnect);
			connect(this, &WebSocketThread::SocketError, this, &WebSocketThread::OnSocketError);
			exec();
		}


		void WebSocketThread::ProcessTextMessage(QString message)
		{
			m_logger->debug("Message received: "); //  << message.toStdString();
			QJsonParseError error;
			QJsonDocument  jsonResponse = QJsonDocument::fromJson(message.toUtf8(), &error);
			if (error.error != QJsonParseError::NoError)
			{
				m_logger->error("Command invalid");
				emit SocketError(nullptr,0);
				return;
			}

			QJsonObject jsonObject = jsonResponse.object();
			QJsonValue valCmd = jsonObject.value("cmd");
			if (valCmd.type() == QJsonValue::Undefined)
			{
				m_logger->error("Command invalid");
				emit SocketError(nullptr, 0);
				return;
			}
			
			CommandIds id = (CommandIds)valCmd.toObject().value("cmdid").toInt();

			AbstractCommand* cmd = m_CmdBuilder->CreateCommand(id, &valCmd.toObject());
			/*Prüfen ob die JSON Anweisung erfolgreich geparst werden konnte*/
			if (cmd == nullptr)
			{
				m_logger->error("Cmdbuilder wasn't able to create the CMD: ");// << (int)id;
				emit SocketError(nullptr, 0);
				return;
			}

			if (cmd->IsParsed())
				emit Message(cmd);
			else
			{
				m_logger->error("Json command couldn't successfuly parsed. CmdID: "); // << (int)id;
				emit SocketError(cmd, 0);
			}

		}

		void WebSocketThread::ProcessBinaryMessage(QByteArray message)
		{
			m_logger->debug("Message received: ");// << message.toStdString();
			emit Message(new AbstractCommand(nullptr));
		}

		void WebSocketThread::OnProcessedMessage(AbstractCommand *Cmd)
		{
			if (Cmd == nullptr)
			{
				m_logger->error("Answer Command was null.");
				return;
			}

			QJsonObject jsonObj;
			jsonObj["success"] = Cmd->Success();
			jsonObj["result"] = QJsonValue::fromVariant(Cmd->Result());
			jsonObj["cmdid"] = Cmd->CommandID();

			QJsonArray jsonArray; 
			jsonArray = QJsonArray::fromVariantList(Cmd->ParameterListe());
			
			jsonObj["param"] = jsonArray;
			jsonObj["executionvariant"] =(int)Cmd->ExecutionVariant();

			QJsonDocument doc;
			doc.setObject(jsonObj);

			m_Socket->sendTextMessage(doc.toJson());

			//Command will not longer needed
			//TODO ist das der einzige fall indem wir dieses objekt löschen müssen?
			delete Cmd;

		}

		void WebSocketThread::OnSocketError(AbstractCommand *Cmd, quint16 Error)
		{
			if (Cmd == nullptr)
			{
				QMetaEnum metaEnum = QMetaEnum::fromType<AbstractCommand::SourceAndDestination>();
				QString source =  metaEnum.valueToKey((int)AbstractCommand::SourceAndDestination::RemoteService);

				//Cmd = new AbstractCommand(CommandIds::FAILURE, source, "", "", , nullptr);
				return;
			}

			QJsonObject jsonObj;
			jsonObj["success"] = false;
			jsonObj["result"] = QJsonValue::fromVariant(Cmd->Result());
			jsonObj["cmdid"] = Cmd->CommandID();

			QJsonArray jsonArray;
			jsonArray = QJsonArray::fromVariantList(Cmd->ParameterListe());

			jsonObj["param"] = jsonArray;
			jsonObj["executionvariant"] = (int)Cmd->ExecutionVariant();

			QJsonDocument doc;
			doc.setObject(jsonObj);

			m_Socket->sendTextMessage(doc.toJson());

			delete Cmd;
		}

		void WebSocketThread::OnDisconnect()
		{
			m_logger->debug("Client disconncted from the WebSocketServer.");
			m_Socket->deleteLater();
			exit(0);
		}
	}
}
