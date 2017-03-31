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
            HANDLE WinApiHelper::GetCurrentUserToken();
            bool WinApiHelper::CreateProcessAsCurrentUser(QString Programm, QString Arguments);
			static bool ReturnCurrentUser(QString &Username);
            bool QueryActiveHW();
		};
	}
}


