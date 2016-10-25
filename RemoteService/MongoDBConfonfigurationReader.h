#pragma once
#include "AbstractSqlConfgurationReader.h"
namespace RW
{
	namespace Config
	{
		class MongoDBConfonfigurationReader :
			public AbstractSqlConfgurationReader
		{
		public:
			MongoDBConfonfigurationReader();
			~MongoDBConfonfigurationReader();
		};

	}
}