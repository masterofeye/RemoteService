#pragma once
#include <QtCore>
#include "spdlog\spdlog.h"
#include "MongoDBWrapper_global.h"

namespace RW{
	namespace MONGO
	{
		/*
		@brief Der Name Base wird hier gewählt aufgrund der Tatsache das grundlegende Mechanismen schon hier implementiert werden.
		*/
		class MONGODBWRAPPER_EXPORT BaseDbObject : public QObject
		{
			Q_OBJECT
		private:

			std::shared_ptr<spdlog::logger> m_logger;
		public:
			BaseDbObject(QObject *Parent = NULL);
			~BaseDbObject();

			void Upload();
			void Download();
		};

	}
}

