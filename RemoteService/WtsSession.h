#pragma once
#include "qobject.h"
namespace RW{
    namespace CORE{
        class WtsSession :
            public QObject
        {
            quint32 m_SessionID = 0;
            QString m_Username = "";
        public:
            WtsSession(QObject* Parent);
            ~WtsSession();
        };

    }
}