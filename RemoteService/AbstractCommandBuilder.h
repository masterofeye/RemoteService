#pragma once
#include "qobject.h"

namespace RW{
	namespace CORE
	{
		class AbstractCommandBuilder :
			public QObject
		{
		public:
			explicit AbstractCommandBuilder(QObject *Parent = nullptr);
			~AbstractCommandBuilder();
		};
	}
}

