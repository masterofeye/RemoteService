#pragma once
#include "qobject.h"



#include "spdlog\spdlog.h"



namespace RW
{
	namespace CORE
	{ 
		class WinApiHelper :
			public QObject
		{
		private: 
			std::shared_ptr<spdlog::logger> m_logger;
		public:
			WinApiHelper();
			~WinApiHelper();

			bool QueryActiveSession(quint64 &SessioNumber);

			static bool ReturnCurrentUser(QString &Username);
		};
	}
}


