#include "ProcessObserver.h"
#include <qmetaobject.h>
#include <qmetatype.h>


ProcessObserver::ProcessObserver(QObject *Parent) : QObject(Parent),
m_ObservedProcess(new QProcess(this)),
m_Logger(spdlog::get("file_logger"))
{
    QObject::connect(m_ObservedProcess, &QProcess::errorOccurred, this, &ProcessObserver::OnError);
    QObject::connect(m_ObservedProcess, SIGNAL(finished), this, SLOT(OnFinished));
    QObject::connect(m_ObservedProcess, &QProcess::started, this, &ProcessObserver::OnStarted);
    QObject::connect(m_ObservedProcess, &QProcess::stateChanged, this, &ProcessObserver::OnStateChanged);
    
}


ProcessObserver::~ProcessObserver()
{
}

void ProcessObserver::SetProgram(const QString &program)
{
    m_ObservedProcess->setProgram(program);
}

void ProcessObserver::Start()
{
    m_ObservedProcess->start();
}

void ProcessObserver::OnError(QProcess::ProcessError error)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ProcessError>();
    m_Logger->error("Process error: {}", metaEnum.valueToKey(error));
}

void ProcessObserver::OnFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QMetaEnum metaEnum = QMetaEnum::fromType< QProcess::ExitStatus>();
    m_Logger->debug("Process closed with status: {} and {}",exitCode, metaEnum.valueToKey(exitStatus));
}

void ProcessObserver::OnStateChanged(QProcess::ProcessState newState)
{
    QMetaEnum metaEnum = QMetaEnum::fromType< QProcess::ProcessState>();
    m_Logger->debug("Process state changed: {}", metaEnum.valueToKey(newState));
}

void ProcessObserver::OnStarted()
{
    m_Logger->debug("Process started.");
}