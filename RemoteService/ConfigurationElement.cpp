#include "ConfigurationElement.h"

namespace RW
{
	namespace Config
	{
		ConfigurationElement::ConfigurationElement(QObject *Parent) : QObject(Parent),
			m_Root(false)
		{
		}


		ConfigurationElement::~ConfigurationElement()
		{
		}
	}
}
