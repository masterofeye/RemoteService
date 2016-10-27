#pragma once
#include "BaseDbObject.h"

namespace RW{
	namespace MYSQL
	{
		class MYSQLDBWRAPPER_EXPORT RemoteWorkstationObject :
			public BaseDbObject
		{
			Q_OBJECT
		public:
			RemoteWorkstationObject();
			~RemoteWorkstationObject();
		};

	}
}