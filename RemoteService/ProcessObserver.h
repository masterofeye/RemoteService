#pragma once
#include "qobject.h"
#include <QProcess>
#include <spdlog\spdlog.h>
class ProcessObserver :
    public QObject
{
private:
    QProcess* m_ObservedProcess;
    std::shared_ptr<spdlog::logger> m_Logger;
public:
    ProcessObserver(QObject *Parent = nullptr);
    ~ProcessObserver();

    void SetProgram(const QString &program);
    void Start();
public slots:
    void OnError(QProcess::ProcessError error);
    void OnFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void OnStateChanged(QProcess::ProcessState newState);
    void OnStarted();
};

