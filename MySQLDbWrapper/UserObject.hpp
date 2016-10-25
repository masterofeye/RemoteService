#pragma once
#include "BaseDbObject.h"

namespace RW{
	namespace MYSQL
	{
		class MYSQLDBWRAPPER_EXPORT UserObject :
			public BaseDbObject
		{
			Q_OBJECT
		public:
			UserObject(QObject *Parent);
			~UserObject();
		};
	}
}