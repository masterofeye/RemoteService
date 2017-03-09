#include "IPCThread.h"
#include <Lmcons.h>

#include "WinApiHelper.h"
#include "CommandInterpreter.hpp"

#include "qfile.h"
#include "qfileinfo.h"
#include <QtXml>

namespace RW{
	namespace CORE
	{
		IPCThread::IPCThread(qintptr ID, QObject *parent) :
			QThread(parent),
			m_logger(spdlog::get("file_logger")),
			m_Command(new CommandInterpreter(this))
		{
			this->socketDescriptor = ID;
			//Q_ASSERT_X(m_logger == nullptr, "BasicServer", "m_logger is null");
		}

		void IPCThread::run()
		{
			// thread starts here
			qDebug() << " Thread started";

			socket = new QTcpSocket();

			// set the ID
			if (!socket->setSocketDescriptor(this->socketDescriptor))
			{
				// something's wrong, we just emit a signal
				emit error(socket->error());
				return;
			}

			// connect socket and signal
			// note - Qt::DirectConnection is used because it's multithreaded
			//        This makes the slot to be invoked immediately, when the signal is emitted.

			connect(socket, SIGNAL(readyRead()), this, SLOT(ReadyRead()), Qt::DirectConnection);
			connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

			// We'll have multiple clients, we want to know which is which
			qDebug() << socketDescriptor << " Client connected";

			// make this thread a loop,
			// thread will stay alive so that signal/slot to function properly
			// not dropped out in the middle when thread dies

			exec();
		}



		void IPCThread::ReadyRead()
		{
			QByteArray a = socket->readAll();
		
			if (QString(a).contains("UserInfo"))
			{
				QByteArray answer;
                QString AC = "", GC = "";
				QString username;
				WinApiHelper::ReturnCurrentUser(username);
				
				answer.append(username);
				answer.append(",");
				answer.append("false");
				answer.append(",");

                QString path = "C:\Program Files (x86)\Schleissheimer\RemoteSoftware\RemoteAPP2\config.xml";

                if (QFileInfo(path).exists())
                {
                    QFile file("path");

                    file.open(QFile::ReadOnly | QFile::Text);
                    QDomDocument doc("mydocument");

                    if (!doc.setContent(&file)) {
                        file.close();
                        return;
                    }
                    file.close();
                   
                    QDomNodeList nodes = doc.elementsByTagName("ACStand");
                    if (nodes.count() == 1 && nodes.at(0).isText())
                    {
                        AC = nodes.at(0).nodeValue();
                    }
                    nodes = doc.elementsByTagName("GCStand");
                    if (nodes.count() == 1)
                    {
                        GC = nodes.at(0).nodeValue();
                    }
                }



				char computername[UNLEN + 1];
				DWORD computernane_len = UNLEN + 1;
				GetComputerNameA(computername, &computernane_len);

				answer.append(computername);
				answer.append(",");
				answer.append("192.156.244.111");
				answer.append(",");
				answer.append("192.156.244.111");
				answer.append(",");
				answer.append("");
				answer.append(",");
				answer.append("Streamlist{}");
                answer.append(AC + "\r\n" + GC);
				socket->write(answer);
			}
		}

		void IPCThread::Disconnected()
		{
			qDebug() << socketDescriptor << " Disconnected";


			socket->deleteLater();
			exit(0);
		}
	}
}