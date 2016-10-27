#pragma once
#include <QtCore>

namespace RW{
	namespace HW{

		enum class State
		{
			ON,
			OFF
		};


		class AbstractDevice : public QObject
		{
			Q_OBJECT
		private:
			QString m_DeviceName;
		public:
			AbstractDevice(QObject *parent = 0);
			~AbstractDevice();

			inline QString DeviceName(){ return m_DeviceName; }

			virtual bool Initialize() = 0;
			virtual bool Reset() = 0;
			virtual bool Shutdown() = 0;
		};
	}
}