#include "AbstractSqlConfgurationReader.h"

namespace RW
{
	namespace Config
	{
		AbstractSqlConfgurationReader::AbstractSqlConfgurationReader(QObject *Parent) : AbstractConfigurationReader(Parent)
		{
		}


		AbstractSqlConfgurationReader::~AbstractSqlConfgurationReader()
		{
		}
	}
}