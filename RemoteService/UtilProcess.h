#pragma once
#include "qobject.h"

QT_BEGIN_NAMESPACE
class QProcess;
QT_END_NAMESPACE

class UtilProcess :
	public QObject
{
public:
	UtilProcess(QObject* Parent = nullptr);
	~UtilProcess();

	static QString StartWithReturn(QString const Path, QStringList *Args = nullptr, bool HideWindow = true, quint64 Timeoute = 30000);
	static bool Start(QString const Path, QStringList *Args = nullptr, bool HideWindow = true, quint64 Timeoute = 30000, QString WorkingDirectory = "");
};

