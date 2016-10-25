#pragma once
#include "BaseDbObject.h"
namespace RW{
	namespace MYSQL
	{
		class MYSQLDBWRAPPER_EXPORT HistoryObject :
			public BaseDbObject
		{
			Q_OBJECT
		public:
			HistoryObject();
			~HistoryObject();
		};
	}
}

