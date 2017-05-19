
#include "ProcessObserverPrivate.h"
#include "ProcessObserver.h"

#include <qdebug.h>
#include <qdir.h>
#if defined(Q_OS_WIN)
#include <qtimer.h>
#endif
#include <ctype.h>
#include "QtCore/qprocess.h"
#include "QtCore/qstringlist.h"
#include "QtCore/qhash.h"
#include "QtCore/qshareddata.h"

#include <qbytearray.h>
#include <qelapsedtimer.h>
#include <qcoreapplication.h>
#include <qsocketnotifier.h>
#include <qtimer.h>
#include <qelapsedtimer.h>
#include <qwineventnotifier.h>
#include "QtCore/qt_windows.h"
#include "WinApiHelper.h"


#include "Userenv.h"
#pragma comment(lib, "userenv.lib")

namespace RW{
    namespace CORE{

        class ProcEnvKey : public QString
        {
        public:
            ProcEnvKey() {}
            explicit ProcEnvKey(const QString &other) : QString(other) {}
            ProcEnvKey(const ProcEnvKey &other) : QString(other) {}
            bool operator==(const ProcEnvKey &other) const { return !compare(other, Qt::CaseInsensitive); }
        };
        inline uint qHash(const ProcEnvKey &key) { return qHash(key.toCaseFolded()); }

        typedef QString ProcEnvValue;



        class ProcessEnvironmentPrivate : public QSharedData
        {
        public:
            typedef ProcEnvKey Key;
            typedef ProcEnvValue Value;

            inline Key prepareName(const QString &name) const { return Key(name); }
            inline QString nameToString(const Key &name) const { return name; }
            inline Value prepareValue(const QString &value) const { return value; }
            inline QString valueToString(const Value &value) const { return value; }
            struct MutexLocker {
                MutexLocker(const ProcessEnvironmentPrivate *) {}
            };
            struct OrderedMutexLocker {
                OrderedMutexLocker(const ProcessEnvironmentPrivate *,
                    const ProcessEnvironmentPrivate *) {}
            };

            typedef QHash<Key, Value> Hash;
            Hash hash;
            static QProcessEnvironment fromList(const QStringList &list);
            QStringList toList() const;
            QStringList keys() const;
            void insert(const ProcessEnvironmentPrivate &other);
        };

        //template<> Q_INLINE_TEMPLATE void QSharedDataPointer<ProcessEnvironmentPrivate>::detach()
        //{
        //    if (d && d->ref.load() == 1)
        //        return;
        //    QProcessEnvironmentPrivate *x = (d ? new QProcessEnvironmentPrivate(*d)
        //        : new QProcessEnvironmentPrivate);
        //    x->ref.ref();
        //    if (d && !d->ref.deref())
        //        delete d;
        //    d = x;
        //}






        ProcessObserverPrivate::ProcessObserverPrivate(ProcessObserver* Observer) :
            q_ptr(Observer)
        {
            m_Logger = spdlog::get("file_logger");
            processError = QProcess::UnknownError;
            processState = QProcess::NotRunning;
            pid = 0;
            sequenceNumber = 0;
            exitCode = 0;
            exitStatus = QProcess::NormalExit;
            startupSocketNotifier = 0;
            deathNotifier = 0;
            crashed = false;
            dying = false;
            processFinishedNotifier = 0;
        }

        ProcessObserverPrivate::~ProcessObserverPrivate()
        {

        }

        void ProcessObserverPrivate::cleanup()
        {
            q_func()->setProcessState(QProcess::NotRunning);
            if (processFinishedNotifier) {
                delete processFinishedNotifier;
                processFinishedNotifier = 0;
            }
            pid = 0;
            sequenceNumber = 0;
            dying = false;

            if (startupSocketNotifier) {
                delete startupSocketNotifier;
                startupSocketNotifier = 0;
            }
            if (deathNotifier) {
                delete deathNotifier;
                deathNotifier = 0;
            }
        }

        void ProcessObserverPrivate::setError(QProcess::ProcessError error, const QString &description)
        {
            processError = error;
            if (description.isEmpty()) {
                switch (error) {
                case QProcess::FailedToStart:
                    errorString = QProcess::tr("Process failed to start");
                    break;
                case QProcess::Crashed:
                    errorString = QProcess::tr("Process crashed");
                    break;
                case QProcess::Timedout:
                    errorString = QProcess::tr("Process operation timed out");
                    break;
                case QProcess::ReadError:
                    errorString = QProcess::tr("Error reading from process");
                    break;
                case QProcess::WriteError:
                    errorString = QProcess::tr("Error writing to process");
                    break;
                case QProcess::UnknownError:
                    errorString.clear();
                    break;
                }
            }
            else {
                errorString = description;
            }
        }

        void ProcessObserverPrivate::setErrorAndEmit(QProcess::ProcessError error, const QString &description)
        {
            Q_Q(ProcessObserver);
            Q_ASSERT(error != QProcess::UnknownError);
            setError(error, description);
            emit q->errorOccurred(processError);
            emit q->error(processError);
        }

        bool ProcessObserverPrivate::_q_processDied()
        {
            Q_Q(ProcessObserver);
#ifdef Q_OS_WIN
            if (processFinishedNotifier)
                processFinishedNotifier->setEnabled(false);
#endif

            // the process may have died before it got a chance to report that it was
            // either running or stopped, so we will call _q_startupNotification() and
            // give it a chance to emit started() or errorOccurred(FailedToStart).
            if (processState == QProcess::Starting) {
                if (!_q_startupNotification())
                    return true;
            }

            if (dying) {
                // at this point we know the process is dead. prevent
                // reentering this slot recursively by calling waitForFinished()
                // or opening a dialog inside slots connected to the readyRead
                // signals emitted below.
                return true;
            }
            dying = true;

            findExitCode();

            if (crashed) {
                exitStatus = QProcess::CrashExit;
                setErrorAndEmit(QProcess::Crashed);
            }

            bool wasRunning = (processState == QProcess::Running);

            cleanup();

            if (wasRunning) {
                emit q->finished(exitCode);
                emit q->finished(exitCode, exitStatus);
            }
            m_Logger->debug("QProcessObserverPrivate::_q_processDied() process is dead");

            return true;
        }

        bool ProcessObserverPrivate::_q_startupNotification()
        {
            Q_Q(ProcessObserver);
            m_Logger->debug("QProcessPrivate::startupNotification()");

            if (startupSocketNotifier)
                startupSocketNotifier->setEnabled(false);
            QString errorMessage;
            if (processStarted(&errorMessage)) {
                q->setProcessState(QProcess::Running);
                emit q->started();
                return true;
            }

            q->setProcessState(QProcess::NotRunning);
            setErrorAndEmit(QProcess::FailedToStart, errorMessage);
#ifdef Q_OS_UNIX
            // make sure the process manager removes this entry
            waitForDeadChild();
            findExitCode();
#endif
            cleanup();
            return false;
        }

        void ProcessObserverPrivate::start()
        {
            exitCode = 0;
            exitStatus = QProcess::NormalExit;
            processError = QProcess::UnknownError;
            errorString.clear();
            startProcess();
        }

        bool ProcessObserverPrivate::waitForStarted(int)
        {
            if (processStarted())
                return true;

            if (processError == QProcess::FailedToStart)
                return false;

            setError(QProcess::Timedout);
            return false;
        }

        bool ProcessObserverPrivate::waitForFinished(int msecs)
        {
            //QIncrementalSleepTimer timer(msecs);

            //forever{
            //    if (!pid) {
            //        return true;
            //    }

            //    if (WaitForSingleObject(pid->hProcess, timer.nextSleepTime()) == WAIT_OBJECT_0) {
            //        if (pid)
            //            _q_processDied();
            //        return true;
            //    }

            //    if (timer.hasTimedOut())
            //        break;
            //}

            //setError(QProcess::Timedout);
            return true;
        }

        static BOOL QT_WIN_CALLBACK qt_terminateApp(HWND hwnd, LPARAM procId)
        {
            DWORD currentProcId = 0;
            GetWindowThreadProcessId(hwnd, &currentProcId);
            if (currentProcId == (DWORD)procId)
                PostMessage(hwnd, WM_CLOSE, 0, 0);

            return TRUE;
        }

        void ProcessObserverPrivate::terminateProcess()
        {
            if (pid) {
                EnumWindows(qt_terminateApp, (LPARAM)pid->dwProcessId);
                PostThreadMessage(pid->dwThreadId, WM_CLOSE, 0, 0);
            }
        }

        void ProcessObserverPrivate::killProcess()
        {
            if (pid)
            {
                bool res = TerminateProcess(pid->hProcess, 0xf291);

                m_Logger->debug("HiddenHelper killed: Status {}",res);
            }
        }

        static QString qt_create_commandline(const QString &program, const QStringList &arguments)
        {
            QString args;
            if (!program.isEmpty()) {
                QString programName = program;
                if (!programName.startsWith(QLatin1Char('\"')) && !programName.endsWith(QLatin1Char('\"')) && programName.contains(QLatin1Char(' ')))
                    programName = QLatin1Char('\"') + programName + QLatin1Char('\"');
                programName.replace(QLatin1Char('/'), QLatin1Char('\\'));

                // add the prgram as the first arg ... it works better
                args = programName + QLatin1Char(' ');
            }

            for (int i = 0; i < arguments.size(); ++i) {
                QString tmp = arguments.at(i);
                // Quotes are escaped and their preceding backslashes are doubled.
                tmp.replace(QRegExp(QLatin1String("(\\\\*)\"")), QLatin1String("\\1\\1\\\""));
                if (tmp.isEmpty() || tmp.contains(QLatin1Char(' ')) || tmp.contains(QLatin1Char('\t'))) {
                    // The argument must not end with a \ since this would be interpreted
                    // as escaping the quote -- rather put the \ behind the quote: e.g.
                    // rather use "foo"\ than "foo\"
                    int i = tmp.length();
                    while (i > 0 && tmp.at(i - 1) == QLatin1Char('\\'))
                        --i;
                    tmp.insert(i, QLatin1Char('"'));
                    tmp.prepend(QLatin1Char('"'));
                }
                args += QLatin1Char(' ') + tmp;
            }
            return args;
        }

        static QByteArray qt_create_environment(const ProcessEnvironmentPrivate::Hash &environment)
        {
            QByteArray envlist;
            if (!environment.isEmpty()) {
                ProcessEnvironmentPrivate::Hash copy = environment;

                // add PATH if necessary (for DLL loading)
                ProcessEnvironmentPrivate::Key pathKey(QLatin1String("PATH"));
                if (!copy.contains(pathKey)) {
                    QByteArray path = qgetenv("PATH");
                    if (!path.isEmpty())
                        copy.insert(pathKey, QString::fromLocal8Bit(path));
                }

                // add systemroot if needed
                ProcessEnvironmentPrivate::Key rootKey(QLatin1String("SystemRoot"));
                if (!copy.contains(rootKey)) {
                    QByteArray systemRoot = qgetenv("SystemRoot");
                    if (!systemRoot.isEmpty())
                        copy.insert(rootKey, QString::fromLocal8Bit(systemRoot));
                }

                int pos = 0;
                ProcessEnvironmentPrivate::Hash::ConstIterator it = copy.constBegin(),
                    end = copy.constEnd();

                static const wchar_t equal = L'=';
                static const wchar_t nul = L'\0';

                for (; it != end; ++it) {
                    uint tmpSize = sizeof(wchar_t) * (it.key().length() + it.value().length() + 2);
                    // ignore empty strings
                    if (tmpSize == sizeof(wchar_t) * 2)
                        continue;
                    envlist.resize(envlist.size() + tmpSize);

                    tmpSize = it.key().length() * sizeof(wchar_t);
                    memcpy(envlist.data() + pos, it.key().utf16(), tmpSize);
                    pos += tmpSize;

                    memcpy(envlist.data() + pos, &equal, sizeof(wchar_t));
                    pos += sizeof(wchar_t);

                    tmpSize = it.value().length() * sizeof(wchar_t);
                    memcpy(envlist.data() + pos, it.value().utf16(), tmpSize);
                    pos += tmpSize;

                    memcpy(envlist.data() + pos, &nul, sizeof(wchar_t));
                    pos += sizeof(wchar_t);
                }
                // add the 2 terminating 0 (actually 4, just to be on the safe side)
                envlist.resize(envlist.size() + 4);
                envlist[pos++] = 0;
                envlist[pos++] = 0;
                envlist[pos++] = 0;
                envlist[pos++] = 0;
            }
            return envlist;
        }

        void ProcessObserverPrivate::startProcess()
        {
            Q_Q(ProcessObserver);

            bool success = false;

            if (pid) {
                CloseHandle(pid->hThread);
                CloseHandle(pid->hProcess);
                delete pid;
                pid = 0;
            }
            pid = new PROCESS_INFORMATION;
            memset(pid, 0, sizeof(PROCESS_INFORMATION));

            q->setProcessState(QProcess::Starting);


            QString args = qt_create_commandline(program, arguments);
            QByteArray envlist;
            //Todo enviroment
            //if (environment.d.constData())
            //    envlist = qt_create_environment(environment.d.constData()->hash);
            if (!nativeArguments.isEmpty()) {
                if (!args.isEmpty())
                    args += QLatin1Char(' ');
                args += nativeArguments;
            }

#if defined QPROCESS_DEBUG
            qDebug("Creating process");
            qDebug("   program : [%s]", program.toLatin1().constData());
            qDebug("   args : %s", args.toLatin1().constData());
            qDebug("   pass environment : %s", environment.isEmpty() ? "no" : "yes");
#endif

            // We cannot unconditionally set the CREATE_NO_WINDOW flag, because this
            // will render the stdout/stderr handles connected to a console useless
            // (this typically affects ForwardedChannels mode).
            // However, we also do not want console tools launched from a GUI app to
            // create new console windows (behavior consistent with UNIX).
            DWORD dwCreationFlags = (GetConsoleWindow() ? 0 : CREATE_NO_WINDOW);
            dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
            STARTUPINFOW startupInfo = { sizeof(STARTUPINFO), 0, 0, 0,
                (ulong)CW_USEDEFAULT, (ulong)CW_USEDEFAULT,
                (ulong)CW_USEDEFAULT, (ulong)CW_USEDEFAULT,
                0, 0, 0,
                STARTF_USESTDHANDLES,
                0, 0, 0,
                0, 0, 0,
            };

            const QString nativeWorkingDirectory = QDir::toNativeSeparators(workingDirectory);
            QProcess::CreateProcessArguments cpargs = {
                0, (wchar_t*)args.utf16(),
                0, 0, TRUE, dwCreationFlags,
                environment.isEmpty() ? 0 : envlist.data(),
                nativeWorkingDirectory.isEmpty() ? Q_NULLPTR : (wchar_t*)nativeWorkingDirectory.utf16(),
                &startupInfo, pid
            };
            if (modifyCreateProcessArgs)
                modifyCreateProcessArgs(&cpargs);

            WinApiHelper helper;
            HANDLE primaryToken = helper.GetCurrentUserToken();
            if (primaryToken == 0)
            {
                success = false;
                errorString = QProcess::tr("Get user token failed: %1").arg(qt_error_string());
            }
            else
            {
                QString errorString;
                void* lpEnvironment = NULL;
                success = CreateEnvironmentBlock(&lpEnvironment, primaryToken, FALSE);

                if (!success) {
                    // Capture the error string before we do CloseHandle below
                    errorString = QProcess::tr("Couldn't create CreateEnvironmentBlock: %1").arg(qt_error_string());
                }
                else
                {

                    QString desctop = "winsta0\\default";
                    LPWSTR lpdesctop = (LPWSTR)desctop.utf16();
                    cpargs.startupInfo->lpDesktop = lpdesctop;

                    success = CreateProcessAsUser(primaryToken, cpargs.applicationName, cpargs.arguments, cpargs.processAttributes, cpargs.threadAttributes, cpargs.inheritHandles,
                        NORMAL_PRIORITY_CLASS |
                        CREATE_UNICODE_ENVIRONMENT, cpargs.environment, cpargs.currentDirectory,
                        cpargs.startupInfo, cpargs.processInformation);



                    //success = CreateProcess(cpargs.applicationName, cpargs.arguments, cpargs.processAttributes,
                    //    cpargs.threadAttributes, cpargs.inheritHandles, cpargs.flags,
                    //    cpargs.environment, cpargs.currentDirectory, cpargs.startupInfo,
                    //    cpargs.processInformation);


                    if (!success) {
                        // Capture the error string before we do CloseHandle below
                        errorString = QProcess::tr("Process failed to start: %1").arg(qt_error_string());
                    }

                }
                DestroyEnvironmentBlock(lpEnvironment);
            }
            if (!success) {
                cleanup();
                setErrorAndEmit(QProcess::FailedToStart, errorString);
                q->setProcessState(QProcess::NotRunning);
                return;
            }

             q->setProcessState(QProcess::Running);
            // User can call kill()/terminate() from the stateChanged() slot
            // so check before proceeding
            if (!pid)
                return;

            //if (threadData->hasEventDispatcher()) {
            //processFinishedNotifier = new QWinEventNotifier(pid->hProcess, q);
            //QObject::connect(processFinishedNotifier, SIGNAL(activated(HANDLE)), q, SLOT(_q_processDied()));
            //processFinishedNotifier->setEnabled(true);
            //}

            _q_startupNotification();
  
            CloseHandle(primaryToken);
        }

        bool ProcessObserverPrivate::processStarted(QString * /*errorMessage*/)
        {
            return processState == QProcess::Running;
        }

        void ProcessObserverPrivate::findExitCode()
        {
            DWORD theExitCode;
            Q_ASSERT(pid);
            if (GetExitCodeProcess(pid->hProcess, &theExitCode)) {
                exitCode = theExitCode;
                crashed = (exitCode == 0xf291   // our magic number, see killProcess
                    || (theExitCode >= 0x80000000 && theExitCode < 0xD0000000));
            }
        }
    
    };
}