#pragma once
#include <QtCore>

namespace RW
{
	namespace Config
	{
		class AbstractConfigurationReader : public QObject
		{
		public:
			explicit AbstractConfigurationReader(QObject *Parent);
			virtual ~AbstractConfigurationReader() = 0;

			virtual bool ReadConfiguration(const QVariant &File) = 0;
			virtual bool WriteConfiguration() = 0;
		};
	}
}