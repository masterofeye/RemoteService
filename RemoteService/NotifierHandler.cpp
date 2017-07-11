#include "NotifierHandler.h"
namespace RW{
    namespace CORE{
        NotifierHandler::NotifierHandler(QObject* Parent) : QObject(Parent),
            m_Logger(spdlog::get("remoteservice"))
        {
        }


        NotifierHandler::~NotifierHandler()
        {
        }

        void NotifierHandler::OnProcessMessage(COM::Message Msg)
        {
            switch (Msg.MessageID())
            {
            case COM::MessageDescription::IN_Notifier:
            {
                QList<QVariant> paramlist = Msg.ParameterList();
                if (paramlist.count() != 4 && paramlist.count() != 3)
                {
                    m_Logger->error("Wrong amount of parameter for IN_Notifier.");
                    return;
                }

                bool isTimeout = false;
                quint64 timeout = 0;
                QString message = "";
                quint8 index = 0;

                isTimeout = paramlist.at(0).toBool();
                timeout = paramlist.at(1).toULongLong();
                message = paramlist.at(2).toString();
                index = paramlist.at(3).toInt();

                COM::Message externalMessage;
                externalMessage.SetIsExternal(true);
                QString identifier = COM::Message::GenUUID(COM::TypeofServer::RemoteHiddenHelper).toString();
                externalMessage.setIdentifier(identifier);
                QList<QVariant> paramlistExternal;
                if (isTimeout)
                {
                    externalMessage.SetMessageID(COM::MessageDescription::EX_ShowPopUpWithTimeout);
                    paramlistExternal.append(message);
                    paramlistExternal.append(timeout);
                    paramlistExternal.append(index);
                }
                else
                {
                    externalMessage.SetMessageID(COM::MessageDescription::EX_ShowPopUp);
                    paramlistExternal.append(message);
                    paramlistExternal.append(index);
                    
                }
                externalMessage.SetParameterList(paramlistExternal);
                emit NewMessage(externalMessage);
            }
            break;
            default:
                break;
            }
        }


    }
}