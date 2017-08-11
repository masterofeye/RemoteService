#pragma once
#include "qobject.h"

namespace RW
{
    namespace CORE
    {
        class ConfigurationManager;
        class WtsSessionManager :
            public QObject
        {
        public:
            WtsSessionManager(ConfigurationManager* Configmanager, QObject* Parent);
            ~WtsSessionManager();
        };

    }
}
