#pragma once
#include <QtCore>
#include <spdlog\spdlog.h>


namespace RW{
	namespace HW{

		enum class PortState
		{
			ON,
			OFF,
			SNA
		};


		class AbstractDevice : public QObject
		{
			Q_OBJECT
		private:
			QString m_DeviceName;
		protected:
			std::shared_ptr<spdlog::logger> m_Logger;
		public:
			AbstractDevice(QObject *parent = 0);
			AbstractDevice(std::shared_ptr<spdlog::logger> Logger, QObject *parent = 0);
			~AbstractDevice();

			inline QString DeviceName(){ return m_DeviceName; }

			virtual bool Initialize() = 0;
			virtual bool Reset() = 0;
			virtual bool Shutdown() = 0;
		};
	}
}