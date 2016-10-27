#pragma once
#include <QObject>

class TestSuite : public QObject
{
    Q_OBJECT
public:
    QString Name;
    static QList<QObject*> INST;
    static QList<QObject*> &TestSuite::mSuites(){ static QList<QObject*> INST; return INST; }
public:
    explicit  TestSuite(QString name) : Name(name){ mSuites().push_back(this); }
    ~TestSuite(){}

};

