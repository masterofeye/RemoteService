#pragma once
#include "AbstractConfigurationReader.h"
#include "spdlog\spdlog.h"

namespace RW
{
	namespace Config
	{


		class Configuration;
		class ConfigurationElement;

		typedef std::function<bool(ConfigurationElement*)> funcptr;

		class XmlConfigurationReader : public AbstractConfigurationReader
		{
		private: 
			QXmlStreamReader* m_StreamReader;
			Configuration* m_Cfg;
			std::shared_ptr<spdlog::logger> m_Logger;

		public:
			XmlConfigurationReader(QObject *Parent);
			~XmlConfigurationReader();


			virtual bool ReadConfiguration(const QVariant &File);
			virtual bool WriteConfiguration();

		private: 
			bool XmlConfigurationReader::Test();

			bool ReadGui(ConfigurationElement *CfgParentElement);
			bool ReadToolBox(ConfigurationElement *CfgParentElement);
			bool ReadTabwidget(ConfigurationElement *CfgParentElement);
			bool ReadPage(ConfigurationElement *CfgParentElement);
			bool ReadTab(ConfigurationElement *CfgParentElement);
			bool ReadGroup(ConfigurationElement *CfgParentElement);
			bool ReadElement(ConfigurationElement *CfgParentElement);
			bool ReadCurrentXMLTag(QString Element, ConfigurationElement *CfgParentElement, funcptr const& Pointer);

			bool XmlConfigurationReader::ReadEl(ConfigurationElement* el);
			bool XmlConfigurationReader::Test(ConfigurationElement *Parent);

		};
	}
}

