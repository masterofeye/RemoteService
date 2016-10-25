#pragma once
#include <QtCore>


class CanEasyHelper : public QObject
{
	Q_OBJECT
public:
	CanEasyHelper();
	~CanEasyHelper();

	bool InitializeCom();

#pragma region Helper Functions
	/*
	@brief Starts CanEasy from the given path. 
	@param [in] CanEasy instalation dir.
	*/
	bool StartCanEasy(QString CanEasyInstallDir = "C:\\Program Files (x86)\\CanEasy\\");

	/*
	@brief Load the given workspace in CanEasy.
	@param Path the workspace, that should be load. 
	*/
	bool LoadWorkSpace(QString WorkspacePath);

	bool StartSimulation();
	bool StopSimulation();
#pragma endregion
signals:
	void CanEasyReady();
public slots:
	void ChangeWorkspace();
	void StopCanEasy();

};

