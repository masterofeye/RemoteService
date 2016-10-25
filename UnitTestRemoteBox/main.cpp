#include <QtTest\QtTest>
#include "TestSuite.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString Path = argv[0];
    QStringList arguments;
    arguments << Path << "-o" << "test.xml,xml" << "-iterations" << "1" << "-v2";
    if (arguments.count() > 1)
    { 
        QDir unitTestPath(argv[1]);
        if (!unitTestPath.exists())
        {
            qDebug() << "UnitTest folder/path didn't exists.";
            return 1;
        }
        else
        {
            QList<QObject*>::iterator it;
            for (it = TestSuite::mSuites().begin(); it != TestSuite::mSuites().end(); it++)
            {
                arguments.replace(2, unitTestPath.path() + "/" + ((TestSuite*)(*it))->Name + ".xml, xml");
                QTest::qExec(*it, arguments);
            }
        }

    }

    return 0;
}