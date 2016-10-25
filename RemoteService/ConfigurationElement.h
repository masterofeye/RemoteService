#pragma once
#include <QtCore>

namespace RW
{
	namespace Config
	{
		class ConfigurationElement : public QObject
		{
		private:
			QString m_Name;
			QString m_Type;
			QHash<QString,QVariant> m_AttributeList;
			QList<ConfigurationElement*> m_Child;
			bool					     m_Root;
		public:
			ConfigurationElement();
			explicit ConfigurationElement(QObject *Parent);
			~ConfigurationElement();

			inline bool IsRoot(){ return m_Root; }
			inline QString& Name(){ return m_Name; };
			inline QString& Type(){ return m_Type; };
			inline QHash<QString, QVariant> & AttributeList(){ return m_AttributeList; };
			inline QList<ConfigurationElement*>* Child(){ return &m_Child; };
			QVariant AttributeByName(QString Key){ return m_AttributeList.value(Key);};

			inline bool SetRoot(){ return m_Root = true; }
			inline void SetType(QString Type){ m_Type = Type; }
			inline void SetName(QString Name){ m_Name = Name; }
			inline void AddAttribute(QString Name, QVariant Value){ m_AttributeList.insert(Name,Value); }
			inline void AddChild(ConfigurationElement* Child){ m_Child.append(Child); }
		};
	}
}
