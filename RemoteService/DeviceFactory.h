#pragma once
#include "qobject.h"
#include <functional>
#include <qhostaddress.h>

namespace RW{
    enum class PeripheralType;
    namespace SQL{
        class Peripheral;
    }
    namespace COM{
        class Message;
    }

    namespace HW
    {
        class AbstractDevice;
        class DeviceFactory :
            public QObject
        {

        public:
            DeviceFactory();
            ~DeviceFactory();

            /*!
            @brief Erzeugt 
            */
            static AbstractDevice* CreateDevice(const RW::SQL::Peripheral &Device,QMap<PeripheralType, AbstractDevice*> *m_DeviceList);

            static void CreateSwitchOnMessage(const RW::SQL::Peripheral &Device, QVector<std::function<bool(void)>> &SwitchOnMessage, QMap<PeripheralType, AbstractDevice*> *m_DeviceList);
        };
    }
}
