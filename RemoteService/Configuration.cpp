#include "Configuration.hpp"

namespace RW
{
	namespace Config
	{
		Configuration::Configuration() : QObject(nullptr), m_Initialized(false)
		{
		}

		void Configuration::SetElementByName(const QString Key, ConfigurationElement * const Element)
		{
			if (Element != nullptr)
			{
				m_currentConfiguration.insert(Key, Element);
			}
		}

		ConfigurationElement* const Configuration::ElementByName(const QString Key) const
		{
			ConfigurationElement *el = nullptr; 
			qDebug() <<m_currentConfiguration.count();
			for each (ConfigurationElement *var in m_currentConfiguration)
			{
				qDebug() << var->Name();
				qDebug() << var->AttributeList();
			}
			el = m_currentConfiguration.value(Key);
			return el;
		}
	}
}