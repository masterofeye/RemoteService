#include "UtilProcess.h"

#include <QProcess>
#include <QDir>
#include <qt_windows.h>

UtilProcess::UtilProcess(QObject* Parent) : QObject(Parent)
{
}


UtilProcess::~UtilProcess()
{
}


QString UtilProcess::StartWithReturn(QString const Path, QStringList *Args, bool HideWindow, quint64 Timeoute)
{
	if (Args != nullptr && Args->count() == 0)
		return nullptr;

	try{
		QProcess process;

		if (HideWindow)
		{
			process.setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args)
			{
				args->flags |= CREATE_NO_WINDOW;
			});
		}

		process.start(Path, *Args);
		
		QByteArray ret = process.readAllStandardOutput();

		bool res = process.waitForFinished(Timeoute);
		if (res)
			return QString(ret);
		else
			return nullptr;
	}
	catch (...)
	{
		return nullptr;
	}
}




bool UtilProcess::Start(QString const Path, QStringList *Args, bool HideWindow, quint64 Timeoute, QString WorkingDirectory)
{
	if (!QDir(WorkingDirectory).exists())
		return false;

	if (Args != nullptr && Args->count() == 0)
		return false;

	try{
		QProcess process;

		if (HideWindow)
		{
			process.setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args)
			{
				args->flags |= CREATE_NO_WINDOW;
			});
		}


		process.start(Path, *Args);

		return process.waitForFinished(Timeoute);
	}
	catch (...)
	{

		return false;
	}
}