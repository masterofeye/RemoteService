#include "FlashCmdProcessor.h"
#include "AbstractCommand.h"

namespace RW{
	namespace PLUGIN
	{
		FlashCmdProcessor::FlashCmdProcessor()
		{
		}


		FlashCmdProcessor::~FlashCmdProcessor()
		{
		}

		void FlashCmdProcessor::DoWork(CORE::AbstractCommand* Cmd)
		{
			Cmd->SetSuccess(true);
			QVariantList param = Cmd->ParameterListe();

			for each (auto var in param)
			{
				QVariantMap map = var.toMap();
				if (map.count() == 0)
				{
					Cmd->SetResult("No flash files");
					Cmd->SetSuccess(false);
					emit finished(Cmd);
					return;
				}
				QVariantMap bootloaderMap = map.value("bootloader").toMap();
				if (bootloaderMap.count() != 0)
				{
					QString bootlaoderPath = bootloaderMap.value("path").toString();
				}
			
				QVariantMap acMap = map.value("ac").toMap();
				if (acMap.count() != 0)
				{
					QString acPath = acMap.value("path").toString();
				}

				QVariantMap gcMap = map.value("gc").toMap();
				if (gcMap.count() != 0)
				{
					QString gcPath = gcMap.value("path").toString();
					bool isUsb = gcMap.value("isusb").toBool();
				}
			}
			emit finished(Cmd);
		}
	}
}
