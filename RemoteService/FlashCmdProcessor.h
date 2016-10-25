#pragma once
#include "qobject.h"



namespace RW{
	namespace CORE
	{
		class AbstractCommand;
	}
	namespace PLUGIN
	{
		class FlashCmdProcessor : public QObject
		{
			Q_OBJECT
		public:
			FlashCmdProcessor();
			~FlashCmdProcessor();
		public slots:
			void DoWork(CORE::AbstractCommand* Cmd);
		signals:
			void finished(CORE::AbstractCommand* Cmd);
		};
	}
}