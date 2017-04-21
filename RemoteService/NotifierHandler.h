#pragma once
#include "qobject.h"
#include "RemoteCommunicationLibrary.h"

namespace RW{
    namespace CORE{
        class NotifierHandler :
            public QObject
        {
            Q_OBJECT
        private: 
            std::shared_ptr<spdlog::logger> m_Logger;
        public:

            NotifierHandler(QObject* Parent = nullptr);
            ~NotifierHandler();
        public slots:
            void OnProcessMessage(COM::Message Msg);
        signals:
            void NewMessage(COM::Message Msg);
        };
    }
}

