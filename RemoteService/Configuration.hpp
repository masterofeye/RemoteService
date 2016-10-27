#pragma once
#include <QMap>
#include "ConfigurationElement.h"

namespace RW
{
	namespace Config
	{
		class Configuration : public QObject
		{
			Q_OBJECT
		private:
			explicit Configuration();
			Configuration(Configuration const&) = delete;       // Copy construct
			Configuration(Configuration&&) = delete;              // Move construct
			Configuration& operator=(Configuration const&) = delete;  // Copy assign
			Configuration& operator=(Configuration &&) = delete;      // Move assign

			QMap<QString, ConfigurationElement*> m_currentConfiguration;
			bool m_Initialized;
		public:
			static Configuration* getInstance()
			{
				// The only instance
				// Guaranteed to be lazy initialized
				// Guaranteed that it will be destroyed correctly
				// C++ 11 Thread Safe
				static Configuration instance;
				return &instance;
			}

			inline bool IsInitialized(){ return m_Initialized; }
			inline void SetIsInitialized(bool Initialized){ m_Initialized = Initialized; }
			void SetElementByName(const QString Key, ConfigurationElement * const Element);
			ConfigurationElement* const ElementByName(const QString Key) const;
			inline QMap<QString, ConfigurationElement*> const& ConfigurationList(){ return m_currentConfiguration; }
		};


	}

}

