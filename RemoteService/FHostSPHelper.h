#pragma once
#include <QtCore>

class FHostSPHelper : public QObject
{
	Q_OBJECT
private:
	quint8 m_Tries;
public:
	FHostSPHelper();
	~FHostSPHelper();

public slots:
	bool StartFHostSP();
	bool LoadPrgFile(QString PathToPrgFile);
	bool ChangePrgFile(QString PathToPrgFile){ return LoadPrgFile(PathToPrgFile); }
	bool StartFHostSPFlashing();
signals:
	void StartFlash();
	void FHostSPFinished();
};

