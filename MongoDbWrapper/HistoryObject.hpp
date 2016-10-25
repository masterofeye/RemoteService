#pragma once
#include "BaseDbObject.h"
namespace RW{
	namespace MONGO
	{
		class MONGODBWRAPPER_EXPORT HistoryObject :
			public BaseDbObject
		{
			Q_OBJECT
		public:
			HistoryObject();
			~HistoryObject();
		};
	}
}

