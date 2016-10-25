#pragma once
#include "AbstractConfigurationReader.h"

namespace RW
{
	namespace Config
	{
		class AbstractSqlConfgurationReader : public AbstractConfigurationReader
		{
		public:
			AbstractSqlConfgurationReader(QObject *Parent);
			~AbstractSqlConfgurationReader();
		};
	}
}

