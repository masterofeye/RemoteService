#pragma once

#include <chrono>
#include <ctime>

#include "spdlog/sinks/sink.h"
#include "MongoDbClient.hpp"
#include "LogObject.h"


namespace spdlog
{
	namespace sinks
	{
		class MongoDbSink : public sink
		{
		private:
			QList<RW::MONGO::LogObject*> *m_Buffer;
		public:
			explicit MongoDbSink()
			{
				m_Buffer = new QList<RW::MONGO::LogObject*>();
			}

			~MongoDbSink()
			{
				delete m_Buffer;
			}

			QString toString(level::level_enum Level)
			{
				switch (Level)
				{
				case level::level_enum::alert:
					return "alert";
				case level::level_enum::critical:
					return "critical";
				case level::level_enum::debug:
					return "debug";
				case level::level_enum::emerg:
					return "emerg";
				case level::level_enum::err:
					return "err";
				case level::level_enum::info:
					return "info";
				case level::level_enum::notice:
					return "notice";
				case level::level_enum::off:
					return "off";
				case level::level_enum::trace:
					return "trace";
				case level::level_enum::warn:
					return "warn";
				}

			}


			void log(const details::log_msg& msg) override
			{
				RW::MONGO::LogObject *obj = new RW::MONGO::LogObject();
				//std::time_t t = std::chrono::system_clock::to_time_t();
				//
				//QDateTime timestamp;
				//timestamp.setTime_t(t);
				//QString s = timestamp.toString("dd.MM.yyyy|hh:mm:ss.zzz");
				std::chrono::system_clock::duration duration{ msg.time.time_since_epoch() };

				TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
				DWORD size = sizeof(computerName) / sizeof(computerName[0]);
				GetComputerName(computerName, &size);
				QString qtComputerName = QString::fromWCharArray(computerName);

				quint64 t = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
				obj->SetDate(t);
				obj->SetMessage(QString::fromStdString(msg.raw.str()));
				obj->SetLogLevel(toString(msg.level));
				obj->SetThreadID((quint16)msg.thread_id);
				obj->SetErrorID(0);
				obj->SetType(QString::fromStdString(msg.type));
				obj->SetComputerN(qtComputerName);
				m_Buffer->append(obj);

				if (msg.level == level::err ||
					msg.level == level::emerg ||
					msg.level == level::critical
					)
					flush();
			}

			void flush()
			{
				for each(RW::MONGO::LogObject* var in *m_Buffer)
				{
					RW::MONGO::MongoDbClient::InsertLog(var);
				}
				//Buffer leeren nachdem alles wegeschrieben wurde
				m_Buffer->clear();
			}

		};
	}
}