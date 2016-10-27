#pragma once
#include "BaseDbObject.h"

namespace RW{
	namespace MYSQL
	{
		class MYSQLDBWRAPPER_EXPORT GeneralObject : public BaseDbObject
		{
		
		private:
			QString m_Version;
			long long m_LogoutTimeout;
			long long m_ShutdownTimeout;

		public:
			GeneralObject(QObject *Parent = NULL);
			~GeneralObject();

			long long LogoutTimout(){ return m_LogoutTimeout; }
			long long ShutdownTimout(){ return m_ShutdownTimeout; }

			void SetLogoutTimout(long long Value){ m_LogoutTimeout = Value;}
			void SetShutdownTimout(long long Value){ m_ShutdownTimeout = Value; }
		};
	}
}

