#pragma once
#include "BaseDbObject.h"
namespace RW{
	namespace MONGO
	{
		class MONGODBWRAPPER_EXPORT SimulationObject :
			public BaseDbObject
		{
			Q_OBJECT
		public:
			SimulationObject();
			~SimulationObject();
		};
	}
}

