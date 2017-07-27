#include "servertestui.h"
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qthread.h>

#include <QBoxLayout>
#include <qlineedit.h>
#include <qcombobox.h>
#include <QtCharts>
#include <qchartview.h>
#include "RemoteDataConnectLibrary.h"


ServerTestUI::ServerTestUI(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    m_logger = spdlog::basic_logger_mt("basic_logger", QApplication::applicationDirPath().toStdString() + "\\basic.txt");
    m_logger->set_level(spdlog::level::trace);


    m_CommunicationServer = new RW::COM::CommunicatonServer(false, COM::TypeofServer::ServiceTest, "Server", 1234, m_logger, nullptr);
    m_CommunicationServer->Register(this);
    m_CommunicationServer->Listen();

    m_MainLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom, this);

    m_Reconnect = new QPushButton();
    m_Reconnect->setText("Reconnect to RemoteService");
    connect(m_Reconnect, &QPushButton::clicked, this, &ServerTestUI::OnReconnect);
    m_Generic = new QBoxLayout(QBoxLayout::Direction::TopToBottom, this);
    m_Generic->addWidget(m_Reconnect);
    m_MainLayout->addLayout(m_Generic);


    m_Logout = new QPushButton();
    m_Logout->setText("LogOut User");
    connect(m_Logout, &QPushButton::clicked, this, &ServerTestUI::OnLogOutUser);
    m_Generic->addWidget(m_Logout);

    m_NotifyLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight,this);

    m_NotifyMessage = new QLineEdit();
    m_NotifyLayout->addWidget(m_NotifyMessage);

    m_NotiferTest = new QPushButton();
    m_NotiferTest->setText("Notify");
    m_ComboBox = new QComboBox();
    m_ComboBox->insertItem(0, "Information");
    m_ComboBox->insertItem(1, "Warning");
    m_ComboBox->insertItem(2, "Alert");
    connect(m_NotiferTest, &QPushButton::clicked, this, &ServerTestUI::OnNotify);
    m_NotifyLayout->addWidget(m_ComboBox);
    m_NotifyLayout->addWidget(m_NotiferTest);
    m_MainLayout->addLayout(m_NotifyLayout);

    QPushButton* m_Anel1 = new QPushButton();
    m_Anel1->setText("Anel1");
    connect(m_Anel1, &QPushButton::clicked, this, &ServerTestUI::OnAnel1);
    QPushButton* m_Anel2 = new QPushButton();
    m_Anel2->setText("Anel2");
    connect(m_Anel2, &QPushButton::clicked, this, &ServerTestUI::OnAnel2);
    QPushButton* m_Anel3 = new QPushButton();
    m_Anel3->setText("Anel3");
    connect(m_Anel3, &QPushButton::clicked, this, &ServerTestUI::OnAnel3);
    QBoxLayout *m_AnelLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);
    m_AnelLayout->addWidget(m_Anel1);
    m_AnelLayout->addWidget(m_Anel2);
    m_AnelLayout->addWidget(m_Anel3);
    m_MainLayout->addLayout(m_AnelLayout);

    QPushButton* m_PowerSupply = new QPushButton();
    m_PowerSupply->setText("PowerSupply");
    QPushButton* m_KL30 = new QPushButton();;
    m_KL30->setText("KL30");
    QBoxLayout *m_PowerSupplyLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);;
    m_PowerSupplyLayout->addWidget(m_PowerSupply);
    m_PowerSupplyLayout->addWidget(m_KL30);
    m_MainLayout->addLayout(m_PowerSupplyLayout);

    QBoxLayout *m_USBLayout0 = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);;;
    QBoxLayout *m_USBLayout1 = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);;;
    QBoxLayout *m_USBLayout2 = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);;;
    QPushButton* m_USB0 = new QPushButton();
    m_USB0->setText("USB0");
    m_USBLayout0->addWidget(m_USB0);
    m_MainLayout->addLayout(m_USBLayout0);

    QPushButton* m_USB1_PORT0 = new QPushButton();
    m_USB1_PORT0->setText("USB1_PORT0");
    QPushButton* m_USB1_PORT1 = new QPushButton();
    m_USB1_PORT1->setText("USB1_PORT1");
    QPushButton* m_USB1_PORT2 = new QPushButton();
    m_USB1_PORT2->setText("USB1_PORT2");
    QPushButton* m_USB1_PORT3 = new QPushButton();
    m_USB1_PORT3->setText("USB1_PORT3");

    m_USBLayout1->addWidget(m_USB1_PORT0);
    m_USBLayout1->addWidget(m_USB1_PORT1);
    m_USBLayout1->addWidget(m_USB1_PORT2);
    m_USBLayout1->addWidget(m_USB1_PORT3);
    m_MainLayout->addLayout(m_USBLayout1);

    QPushButton* m_USB2_PORT0 = new QPushButton();
    m_USB2_PORT0->setText("USB2_PORT0");
    QPushButton* m_USB2_PORT1 = new QPushButton();
    m_USB2_PORT1->setText("USB2_PORT1");
    QPushButton* m_USB2_PORT2 = new QPushButton();
    m_USB2_PORT2->setText("USB2_PORT2");
    QPushButton* m_USB2_PORT3 = new QPushButton();
    m_USB2_PORT3->setText("USB2_PORT3");

    m_USBLayout2->addWidget(m_USB2_PORT0);
    m_USBLayout2->addWidget(m_USB2_PORT1);
    m_USBLayout2->addWidget(m_USB2_PORT2);
    m_USBLayout2->addWidget(m_USB2_PORT3);
    m_MainLayout->addLayout(m_USBLayout2);

    QTabWidget* tab = (QTabWidget*)ui.centralWidget->findChild<QTabWidget*>();
    tab->widget(0)->setLayout(m_MainLayout);

    CreateDatabaseTab(tab->widget(1));

}

ServerTestUI::~ServerTestUI()
{

}

void ServerTestUI::CreateDatabaseTab(QWidget* Widget)
{
    QVBoxLayout* vbox = new QVBoxLayout(Widget);
    
    QComboBox* selectPc = new QComboBox(Widget);
    vbox->addWidget(selectPc);
    selectPc->insertItems(0, GetPCLogEntryList());
    connect(selectPc, SIGNAL(activated(QString)), this, SLOT(FillGraphicForPcState(QString)));

    m_ChartView = new QChartView(Widget);
    m_ChartView->setRenderHint(QPainter::Antialiasing);

    vbox->addWidget(m_ChartView);

    Widget->setLayout(vbox);
}

QStringList ServerTestUI::GetPCLogEntryList()
{
    RW::SQL::Repository* m_rep = new RW::SQL::Repository(RW::SourceType::SQL,this);
    QList<RW::SQL::LogEntry> logEntryList;
    m_rep->GetAllLogEntry(logEntryList);

    QStringList list;
    for each (auto var in logEntryList)
    {
        list.append(var.ComputerNameRW());
    }
    list.removeDuplicates();
    delete m_rep;
    return list;
}

void ServerTestUI::FillGraphicForPcState(QString Hostname)
{
    RW::SQL::Repository* m_rep = new RW::SQL::Repository(RW::SourceType::SQL, this);
    QList<RW::SQL::LogEntry> logEntryList;
    m_rep->GetLogEntryByHostName(Hostname,logEntryList);

    qint8 lastVal = 0;
    QLineSeries* serie = new QLineSeries(this);
    for each (auto var in logEntryList)
    {
        serie->append(var.Date().toMSecsSinceEpoch(), lastVal);
        switch (var.Filter())
        {
        case spdlog::sinks::FilterType::RemoteServiceStart:
            serie->append(var.Date().toMSecsSinceEpoch(), 1);
            lastVal = 1;

        case spdlog::sinks::FilterType::RemoteServiceStop:
            serie->append(var.Date().toMSecsSinceEpoch(), -1);
            lastVal = -1;
            break;
        case spdlog::sinks::FilterType::RemoteServiceConnect:
            serie->append(var.Date().toMSecsSinceEpoch(), 4);
            lastVal = 4;
            break;
            
        case spdlog::sinks::FilterType::RemoteServiceDisconnect:
            serie->append(var.Date().toMSecsSinceEpoch(), 3);
            lastVal = 3;
            break;
        case spdlog::sinks::FilterType::RemoteServiceSessionLogon:
            serie->append(var.Date().toMSecsSinceEpoch(), 5);
            lastVal = 5;
            break;
        case spdlog::sinks::FilterType::RemoteServiceSessionLogoff:
            serie->append(var.Date().toMSecsSinceEpoch(), 2);
            lastVal = 2;
            break;
        default:
            break;
        }

    }

   
    QChart* chart = new QChart();
    chart->addSeries(serie);
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("HH:mm:ss");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    serie->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Sunspots count");
    chart->addAxis(axisY, Qt::AlignLeft);
    serie->attachAxis(axisY);



    
    
    m_ChartView->setChart(chart);


    delete m_rep;
}


void ServerTestUI::OnNotify()
{
    RW::COM::Message m;
    m.SetMessageID(RW::COM::MessageDescription::IN_Notifier);
    bool isTimerout = false;
    quint64 timeout = 0;
    QString Message = "Das ist eine Testnachricht";
    QList<QVariant> parameter;
    parameter.append(isTimerout);
    parameter.append(timeout);
    parameter.append(m_NotifyMessage->text());
    parameter.append(m_ComboBox->currentIndex());
    m.SetParameterList(parameter);
    m.SetIsExternal(true);
    emit NewMessage(m);
} 

void ServerTestUI::OnAnel1()
{
    RW::COM::Message m;
    m.SetMessageID(RW::COM::MessageDescription::IN_POWERSTRIPE);
    QList<QVariant> parameter;
    bool state = true;
    int port = 1;

    parameter.append(port);
    parameter.append(state);
    m.SetParameterList(parameter);
    m.SetIsExternal(true);
    m.SetExcVariant(RW::COM::Message::ExecutionVariant::SET);
    emit NewMessage(m);
}

void ServerTestUI::OnAnel2()
{
    RW::COM::Message m;
    m.SetMessageID(RW::COM::MessageDescription::IN_POWERSTRIPE);
    QList<QVariant> parameter;
    bool state = true;
    int port = 2;

    parameter.append(port);
    parameter.append(state);
    m.SetParameterList(parameter);
    m.SetIsExternal(true);
    m.SetExcVariant(RW::COM::Message::ExecutionVariant::SET);
    emit NewMessage(m);
}

void ServerTestUI::OnAnel3()
{
    RW::COM::Message m;
    m.SetMessageID(RW::COM::MessageDescription::IN_POWERSTRIPE);
    QList<QVariant> parameter;
    bool state = true;
    int port = 3;

    parameter.append(port);
    parameter.append(state);
    m.SetParameterList(parameter);
    m.SetIsExternal(true);
    m.SetExcVariant(RW::COM::Message::ExecutionVariant::SET);
    emit NewMessage(m);
}

void ServerTestUI::OnReconnect(){
    m_CommunicationServer = new RW::COM::CommunicatonServer(false, COM::TypeofServer::ServiceTest, "Server", 1234, m_logger, nullptr);
    m_CommunicationServer->Register(this);
    m_CommunicationServer->Listen();
}

void ServerTestUI::OnProcessMessage(RW::COM::Message Msg)
{

}

void ServerTestUI::OnLogOutUser()
{
    RW::COM::Message m;
    m.SetMessageID(RW::COM::MessageDescription::IN_LogoutImmediately);
    m.SetIsExternal(true);
    m.SetExcVariant(RW::COM::Message::ExecutionVariant::NON);
    m.setIdentifier(COM::Message::GenUUID(COM::TypeofServer::ServiceTest).toString());
    emit NewMessage(m);
}


