#pragma once
#include <QtTest/QtTest>
#include "TestSuite.hpp"
#include "RemoteBoxWrapper.h"

#define NAME "RemoteBoxWrapperTest"

class RemoteBoxWrapperTest : public TestSuite
{
    Q_OBJECT
private: 
    RemoteBoxWrapper::Wrapper* m_Wrapper;
public:
    RemoteBoxWrapperTest() : TestSuite(NAME)
    {
        m_Wrapper = new RemoteBoxWrapper::Wrapper();
    }
private slots :
    void initTestCase()
    {
        bool success = m_Wrapper->Init();
        QVERIFY(success);
    }

    void init()
    {
    }

    void SetDIOPinState_ON_Positiv()
    {
        bool on = true;
        QVERIFY(m_Wrapper->SetDIOPinState(0x01, on));
    }

    void GetDIOPinState_Positiv()
    {
        bool on = false;
        QVERIFY(m_Wrapper->GetDIOPinState(0x01, on));
        QVERIFY(on);
    }


    void SetDIOPinState_OFF_Positiv()
    {
        bool on = false;
        QVERIFY(m_Wrapper->SetDIOPinState(0x01, on));
        QVERIFY(m_Wrapper->GetDIOPinState(0x01, on));
        QVERIFY(!on);
    }

    void SetRelay_ON_Positiv()
    {
        bool on = true;
        QVERIFY(m_Wrapper->SetRelayState(0x01));
    }

    void GetRelayState_Positiv()
    {
        unsigned short state = 0;
        QVERIFY(m_Wrapper->GetRelayState(state));
        QCOMPARE(QTest::toString(state), QTest::toString(0x01));
    }

    void SetRelay_OFF_Positiv()
    {
        unsigned short state = 0;
        QVERIFY(m_Wrapper->SetRelayState(0x00));
        QVERIFY(m_Wrapper->GetRelayState(state));
        QCOMPARE(QTest::toString(state), QTest::toString(0x00));
    }

    void SetUsbState_ON_Positiv()
    {
        QVERIFY(m_Wrapper->SetUsbState(0x01, 0x01));

    }

    void GetUsbState_Positiv()
    {
        unsigned char state = 0;
        QVERIFY(m_Wrapper->GetUsbState(0x01, state));
        QCOMPARE(QTest::toString(state), QTest::toString(0x01));
    }

    void SetUsbState_OFF_Positiv()
    {
        unsigned char state = 0;
        QVERIFY(m_Wrapper->SetUsbState(0x01, 0x00));
        QCOMPARE(QTest::toString(state), QTest::toString(0x00));
    }

    void SetPwmValue_200_Positiv()
    {
        QVERIFY(m_Wrapper->SetPwmValue(0x00, 200));
    }

    void GetPwmValue_200_Positiv()
    {
        unsigned short value = 0;
        QVERIFY(m_Wrapper->GetPwmValue(0x00, value));
        QCOMPARE(QTest::toString(value), QTest::toString(200));
    }

    void SetPwmValue_0_Positiv()
    {
        unsigned short value = 0;
        QVERIFY(m_Wrapper->SetPwmValue(0x00, 0));
        QVERIFY(m_Wrapper->GetPwmValue(0x00, value));
        QCOMPARE(QTest::toString(value), QTest::toString(0));
    }

    void SetADCValue_200_Positiv()
    {
        QVERIFY(m_Wrapper->SetDacValue(0x00,200));
    }

    void GetADCValue_0_Positiv()
    {
        short value = 0;
        QVERIFY(m_Wrapper->GetAdcValue(0x01, value));
        QCOMPARE(QTest::toString(value), QTest::toString(0));
    }

    void SetADCValue_0_Positiv()
    {
        short value = 0;
        QVERIFY(m_Wrapper->SetDacValue(0x00, 0));
    }

    void GetHardwareSerialNumber_Positiv()
    {
        char* data = nullptr;
        QVERIFY(m_Wrapper->GetHardwareSerialNumber(data));
    }

    void cleanup()
    {

    }


    void cleanupTestCase()
    {
        m_Wrapper->DeInit();
    }
};

static RemoteBoxWrapperTest instance;