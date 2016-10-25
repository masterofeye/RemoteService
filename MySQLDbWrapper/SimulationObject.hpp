#pragma once
#include "BaseDbObject.h"
namespace RW{
	namespace MYSQL
	{
		class MYSQLDBWRAPPER_EXPORT SimulationObject :
			public BaseDbObject
		{
			Q_OBJECT
		public:
			SimulationObject();
			~SimulationObject();
		};
	}
}

