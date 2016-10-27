#pragma once
#include "BaseDbObject.h"

namespace RW{
	namespace MYSQL
	{
		class MYSQLDBWRAPPER_EXPORT LogObject :
			public BaseDbObject
		{
		private:
			quint64 m_Date;
			QString m_LogLevel;
			QString m_Type;
			quint16 m_ThreadID;
			quint16 m_ErrorID;
			QString m_Message;
			QString m_ComputerName;

		public:
			LogObject();
			~LogObject();

			void SetDate(quint64 Date){ m_Date = Date; }
			void SetLogLevel(QString Level){ m_LogLevel = Level; }
			void SetType(QString Type){ m_Type = Type; }
			void SetThreadID(quint16 ThreadID){ m_ThreadID = ThreadID; }
			void SetErrorID(quint16 ErrorID){ m_ErrorID = ErrorID; }
			void SetMessage(QString Message){ m_Message = Message; }
			void SetComputerN(QString ComputerName){ m_ComputerName = ComputerName; }
			

			quint64 Date(){ return m_Date; }
			QString LogLevel(){ return m_LogLevel; }
			QString Type(){ return m_Type; }
			quint16 ThreadId(){ return m_ThreadID; }
			quint16 ErrorID(){ return m_ErrorID; }
			QString Message(){ return m_Message; }
			QString ComputerName(){ return m_ComputerName; }

		};
	}
}

