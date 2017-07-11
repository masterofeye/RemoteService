#include "ProcessObserver.h"
#include "ProcessObserverPrivate.h"
#include <qmetaobject.h>
#include <qmetatype.h>
#include <qelapsedtimer.h>
#include "Types.h"

namespace RW{
    namespace CORE{
        ProcessObserver::ProcessObserver(bool ShouldRestart, QObject *Parent) : QObject(Parent),
            m_Logger(spdlog::get("remoteservice")),
            d_ptr(new ProcessObserverPrivate(this)),
            m_ShouldRestart(ShouldRestart)
        {
            if (m_ShouldRestart)
                QObject::connect(this, SIGNAL(finished(int , QProcess::ExitStatus )), this, SLOT(RestartApplication(int, QProcess::ExitStatus)));
        }


        ProcessObserver::~ProcessObserver()
        {
            Q_D(ProcessObserver);
            if (d->processState != QProcess::NotRunning) {
                kill();
                waitForFinished();
            }
            d->cleanup();
        }

        QString ProcessObserver::nativeArguments() const
        {
            Q_D(const ProcessObserver);
            return d->nativeArguments;
        }
        
        void ProcessObserver::setNativeArguments(const QString &arguments)
        {
            Q_D(ProcessObserver);
            d->nativeArguments = arguments;
        }

        ProcessObserver::CreateProcessArgumentModifier ProcessObserver::createProcessArgumentsModifier() const
        {
            Q_D(const ProcessObserver);
            return d->modifyCreateProcessArgs;
        }

        void ProcessObserver::setCreateProcessArgumentsModifier(CreateProcessArgumentModifier modifier)
        {
            Q_D(ProcessObserver);
            d->modifyCreateProcessArgs = modifier;
        }

        QString ProcessObserver::workingDirectory() const
        {
            Q_D(const ProcessObserver);
            return d->workingDirectory;
        }

        void ProcessObserver::setWorkingDirectory(const QString &dir)
        {
            Q_D(ProcessObserver);
            d->workingDirectory = dir;
        }

        Q_PID ProcessObserver::pid() const // ### Qt 6 remove or rename this method to processInformation()
        {
            Q_D(const ProcessObserver);
            return d->pid;
        }

        qint64 ProcessObserver::processId() const
        {
            Q_D(const ProcessObserver);
            return d->pid ? d->pid->dwProcessId : 0;
        }

        void ProcessObserver::close()
        {
            //Q_D(ProcessObserver);
            kill();
        }

        QProcess::ProcessError ProcessObserver::error() const
        {
            Q_D(const ProcessObserver);
            return d->processError;
        }

        QProcess::ProcessState ProcessObserver::state() const
        {
            Q_D(const ProcessObserver);
            return d->processState;
        }

        void ProcessObserver::setEnvironment(const QStringList &environment)
        {
//            setProcessEnvironment(QProcessEnvironmentPrivate::fromList(environment));
            Q_UNUSED(environment);
        }

        QStringList ProcessObserver::environment() const
        {
            Q_D(const ProcessObserver);
            return d->environment.toStringList();
        }

        void ProcessObserver::setProcessEnvironment(const QProcessEnvironment &environment)
        {
            Q_D(ProcessObserver);
            d->environment = environment;
        }

        QProcessEnvironment ProcessObserver::processEnvironment() const
        {
            Q_D(const ProcessObserver);
            return d->environment;
        }

        void ProcessObserver::setProcessState(QProcess::ProcessState state)
        {
            Q_D(ProcessObserver);
            if (d->processState == state)
                return;
            d->processState = state;
            emit stateChanged(state);
        }

        void ProcessObserver::start(const QString &program, const QStringList &arguments)
        {
            Q_D(ProcessObserver);
            if (d->processState != QProcess::NotRunning) {
                m_Logger->warn("QProcess::start: Process is already running", (int)spdlog::sinks::FilterType::ProcessObserver);
                return;
            }
            if (program.isEmpty()) {
                d->setErrorAndEmit(QProcess::FailedToStart, tr("No program defined"));
                return;
            }

            d->program = program;
            d->arguments = arguments;

            d->start();
        }

        void ProcessObserver::start()
        {
            Q_D(ProcessObserver);
            if (d->processState != QProcess::NotRunning) {
                m_Logger->warn("QProcess::start: Process is already running", (int)spdlog::sinks::FilterType::ProcessObserver);
                return;
            }

            if (d->program.isEmpty()) {
                m_Logger->warn("No program defined", (int)spdlog::sinks::FilterType::ProcessObserver);
                d->setErrorAndEmit(QProcess::FailedToStart, tr("No program defined"));
                return;
            }

            d->start();
            m_Logger->warn("RemoteHiddenHelper started", (int)spdlog::sinks::FilterType::ProcessObserver);
        }

        static QStringList parseCombinedArgString(const QString &program)
        {
            QStringList args;
            QString tmp;
            int quoteCount = 0;
            bool inQuote = false;

            // handle quoting. tokens can be surrounded by double quotes
            // "hello world". three consecutive double quotes represent
            // the quote character itself.
            for (int i = 0; i < program.size(); ++i) {
                if (program.at(i) == QLatin1Char('"')) {
                    ++quoteCount;
                    if (quoteCount == 3) {
                        // third consecutive quote
                        quoteCount = 0;
                        tmp += program.at(i);
                    }
                    continue;
                }
                if (quoteCount) {
                    if (quoteCount == 1)
                        inQuote = !inQuote;
                    quoteCount = 0;
                }
                if (!inQuote && program.at(i).isSpace()) {
                    if (!tmp.isEmpty()) {
                        args += tmp;
                        tmp.clear();
                    }
                }
                else {
                    tmp += program.at(i);
                }
            }
            if (!tmp.isEmpty())
                args += tmp;

            return args;
        }

        QString ProcessObserver::program() const
        {
            Q_D(const ProcessObserver);
            return d->program;
        }

        void ProcessObserver::setProgram(const QString &program)
        {
            Q_D(ProcessObserver);
            if (d->processState != QProcess::NotRunning) {
                m_Logger->warn("QProcess::setProgram: Process is already running", (int)spdlog::sinks::FilterType::ProcessObserver);
                return;
            }
            d->program = program;
        }

        QStringList ProcessObserver::arguments() const
        {
            Q_D(const ProcessObserver);
            return d->arguments;
        }

        void ProcessObserver::setArguments(const QStringList &arguments)
        {
            Q_D(ProcessObserver);
            if (d->processState != QProcess::NotRunning) {
                m_Logger->warn("QProcess::start: Process is already running", (int)spdlog::sinks::FilterType::ProcessObserver);
                return;
            }
            d->arguments = arguments;
        }

        void ProcessObserver::terminate()
        {
            Q_D(ProcessObserver);
            d->terminateProcess();
        }

        void ProcessObserver::kill()
        {
            Q_D(ProcessObserver);
            d->killProcess();
            
        }

        int ProcessObserver::exitCode() const
        {
            Q_D(const ProcessObserver);
            return d->exitCode;
        }

        QProcess::ExitStatus ProcessObserver::exitStatus() const
        {
            Q_D(const ProcessObserver);
            return d->exitStatus;
        }


        QStringList ProcessObserver::systemEnvironment()
        {
            QStringList tmp;
            char *entry = 0;
            int count = 0;
            while ((entry = environ[count++]))
                tmp << QString::fromLocal8Bit(entry);
            return tmp;
        }

        QString ProcessObserver::nullDevice()
        {
            return QStringLiteral("\\\\.\\NUL");
        }

        bool ProcessObserver::waitForStarted(int msecs)
        {
            Q_D(ProcessObserver);
            if (d->processState == QProcess::Starting)
                return d->waitForStarted(msecs);

            return d->processState == QProcess::Running;
        }

        int qt_subtract_from_timeout(int timeout, int elapsed)
        {
            if (timeout == -1)
                return -1;

            timeout = timeout - elapsed;
            return timeout < 0 ? 0 : timeout;
        }


        bool ProcessObserver::waitForFinished(int msecs)
        {
            Q_D(ProcessObserver);
            if (d->processState == QProcess::NotRunning)
                return false;
            if (d->processState == QProcess::Starting) {
                QElapsedTimer stopWatch;
                stopWatch.start();
                bool started = waitForStarted(msecs);
                if (!started)
                    return false;
                msecs = qt_subtract_from_timeout(msecs, stopWatch.elapsed());
            }

            return d->waitForFinished(msecs);
        }

        void ProcessObserver::RestartApplication(int ExitCode, QProcess::ExitStatus ExitStatus)
        {
            switch (ExitStatus)
            {
            case QProcess::NormalExit:
                m_Logger->debug("HiddenHelper has ended normaly. Exitcode: {}", (int)spdlog::sinks::FilterType::ProcessObserver, ExitCode);
                break;
            case QProcess::CrashExit:
                m_Logger->debug("HiddenHelper crashed. Exitcode: {}", (int)spdlog::sinks::FilterType::ProcessObserver, ExitCode);
                close();
                //TODO aktuell hard coded
                setProgram("RemoteHiddenHelper.exe");
                start();
                break;
            default:
                break;
             
            }
            m_Logger->flush();
        }
    }
}