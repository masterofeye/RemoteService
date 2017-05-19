#pragma once
#include <spdlog\spdlog.h>
#include <QtCore/qiodevice.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qshareddata.h>
#include <qprocess.h>


namespace RW{
    namespace CORE{

        class ProcessObserverPrivate;

        class ProcessObserver :
            public QObject
        {
            Q_OBJECT
        private:
            std::shared_ptr<spdlog::logger> m_Logger;
            ProcessObserverPrivate* d_ptr;
            Q_DECLARE_PRIVATE(ProcessObserver);
            bool m_ShouldRestart = false;
        public:
            explicit ProcessObserver(bool ShouldRestart, QObject *Parent = nullptr);
            virtual ~ProcessObserver();

            void start(const QString &program, const QStringList &arguments);
            void start();

            QString program() const;
            void setProgram(const QString &program);

            QStringList arguments() const;
            void setArguments(const QStringList & arguments);



            typedef std::function<void(QProcess::CreateProcessArguments *)> CreateProcessArgumentModifier;
            CreateProcessArgumentModifier createProcessArgumentsModifier() const;
            void setCreateProcessArgumentsModifier(CreateProcessArgumentModifier modifier);

            QString nativeArguments() const;
            void setNativeArguments(const QString &arguments);

            QString workingDirectory() const;
            void setWorkingDirectory(const QString &dir);

            void setEnvironment(const QStringList &environment);
            QStringList environment() const;
            void setProcessEnvironment(const QProcessEnvironment &environment);
            QProcessEnvironment processEnvironment() const;

            QProcess::ProcessError error() const;
            QProcess::ProcessState state() const;

            Q_PID pid() const;
            qint64 processId() const;

            int exitCode() const;
            QProcess::ExitStatus exitStatus() const;


            static QStringList systemEnvironment();

            static QString nullDevice();

            bool waitForStarted(int msecs = 30000);
            bool waitForFinished(int msecs = 30000);
            void close();

        public Q_SLOTS:
            void terminate();
            void kill();

        Q_SIGNALS:
            void started();
            void finished(int exitCode); // ### Qt 6: merge the two signals with a default value
            void finished(int exitCode, QProcess::ExitStatus exitStatus);
#if QT_DEPRECATED_SINCE(5,6)
            void error(QProcess::ProcessError error);
#endif
            void errorOccurred(QProcess::ProcessError error);
            void stateChanged(QProcess::ProcessState state);

        protected:
            void setProcessState(QProcess::ProcessState);

        private:
            Q_DISABLE_COPY(ProcessObserver)
#if QT_VERSION <= QT_VERSION_CHECK(5,0,0)
                Q_PRIVATE_SLOT(d_func(), bool _q_startupNotification())
                Q_PRIVATE_SLOT(d_func(), bool _q_processDied())
#endif
            void RestartApplication(int, QProcess::ExitStatus);
            friend class QProcessManager;
        };
    }
}
