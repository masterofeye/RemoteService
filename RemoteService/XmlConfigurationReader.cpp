#include "XmlConfigurationReader.h"

#include <QXmlStreamReader>

#include "Configuration.hpp"
#include "ConfigurationElement.h"

#define TRACE_XML 1
using namespace std::placeholders;
namespace RW
{
	namespace Config
	{


		XmlConfigurationReader::XmlConfigurationReader(QObject *Parent) : AbstractConfigurationReader(Parent),
			m_Cfg(Configuration::getInstance())
		{
			m_StreamReader = new QXmlStreamReader();
			m_Logger = spdlog::get("console");
		}


		XmlConfigurationReader::~XmlConfigurationReader()
		{
			delete m_StreamReader;
		}



		bool XmlConfigurationReader::Test(ConfigurationElement *Parent)
		{
			QXmlStreamReader::TokenType type = QXmlStreamReader::NoToken;
			while ((type = m_StreamReader->readNext()) != QXmlStreamReader::EndDocument && (!m_StreamReader->hasError()))
			{


				if (type == QXmlStreamReader::StartElement)
				{
					ConfigurationElement *el = new ConfigurationElement(m_Cfg);
					if (el == nullptr){ return false; }
					QString Name = "";
					el->SetType(m_StreamReader->name().toString());
					if (Parent != nullptr)
					{
						m_Logger->debug("Name: ") << Parent->Name().toStdString() << " Type: " << Parent->Type().toStdString();
						Parent->AddChild(el);
					}
					QXmlStreamAttributes attrList = m_StreamReader->attributes();
					for (QXmlStreamAttribute var : attrList)
					{
						if (var.name() == "name")
						{
							Name = var.value().toString();
							el->SetName(Name);
						}
						else if (var.name() == "isRoot")
						{
							el->SetRoot();
						}
						el->AddAttribute(var.name().toString(), var.value().toString());
					}
					m_Logger->debug(m_StreamReader->name().toString().toStdString());
					Test(el);

					m_Cfg->SetElementByName(Name, el);
				}


				if (type == QXmlStreamReader::EndElement) //Current Element hat keine Unterelemente mehr also können wir hier abbrechen
				{

					break;
				}
			}

		}




		bool XmlConfigurationReader::ReadConfiguration(const QVariant &Input)
		{

			QFile file((QString)Input.toString());
			ConfigurationElement *parent = nullptr;
			QList<Config::ConfigurationElement*> parentBuffer;
			if(file.open(QIODevice::ReadOnly))
			{
				m_StreamReader->setDevice(&file);

				Test(parent);
				m_Cfg->SetIsInitialized(true);
				return true;
			}
			m_Logger->error("Couldn't open configuration File");
			return false;
		}


		bool XmlConfigurationReader::WriteConfiguration()
		{
			return false;
		}
	}
}