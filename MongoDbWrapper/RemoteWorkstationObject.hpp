#pragma once
#include "BaseDbObject.h"

namespace RW{
	namespace MONGO
	{
		class MONGODBWRAPPER_EXPORT RemoteWorkstationObject :
			public BaseDbObject
		{
			Q_OBJECT
		public:
			RemoteWorkstationObject();
			~RemoteWorkstationObject();
		};

	}
}