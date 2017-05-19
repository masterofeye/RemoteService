#ifndef ProcessObserverPrivate_H
#define ProcessObserverPrivate_H

#include "QtCore/qprocess.h"
#include "QtCore/qstringlist.h"
#include "QtCore/qhash.h"
#include "QtCore/qshareddata.h"
#include <spdlog\spdlog.h>

class QSocketNotifier;
class QWinEventNotifier;
class QTimer;
namespace RW{
    namespace CORE{

        class ProcessObserver;

        class ProcessObserverPrivate
        {
        private:
            ProcessObserver *const q_ptr;
        public:
            Q_DECLARE_PUBLIC(ProcessObserver)
            ProcessObserverPrivate(ProcessObserver* Observer);
            virtual ~ProcessObserverPrivate();

            bool _q_startupNotification();
            bool _q_processDied();

            QProcess::ProcessError processError;
            QProcess::ProcessState processState;
            QString workingDirectory;
            Q_PID pid;
            int sequenceNumber;
            bool dying;

            QString program;
            QStringList arguments;

            QSocketNotifier *startupSocketNotifier;
            QSocketNotifier *deathNotifier;

            QString nativeArguments;
            QProcess::CreateProcessArgumentModifier modifyCreateProcessArgs;
            QProcessEnvironment environment;
            QWinEventNotifier *processFinishedNotifier;
            int exitCode;
            QProcess::ExitStatus exitStatus;
            bool crashed;
            QString errorString;
        public:
            void start();
            void startProcess();
            bool processStarted(QString *errorMessage = Q_NULLPTR);
            void terminateProcess();
            void killProcess();
            void findExitCode();

            void cleanup();
            void setError(QProcess::ProcessError error, const QString &description = QString());
            void setErrorAndEmit(QProcess::ProcessError error, const QString &description = QString());
            bool waitForStarted(int);
            bool waitForFinished(int msecs);
        private:
            std::shared_ptr<spdlog::logger> m_Logger;
        };
    }
}
#endif // ProcessObserverPrivate_H
