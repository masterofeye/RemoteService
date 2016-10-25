#pragma once
#include "BaseDbObject.h"

namespace RW{
	namespace MONGO
	{
		class MONGODBWRAPPER_EXPORT UserObject :
			public BaseDbObject
		{
			Q_OBJECT
		public:
			UserObject(QObject *Parent);
			~UserObject();
		};
	}
}