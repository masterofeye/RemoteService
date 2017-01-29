#include "CanEasyHelper.h"
#include <comdef.h>

CanEasyHelper::CanEasyHelper()
{

}


CanEasyHelper::~CanEasyHelper()
{
}

bool CanEasyHelper::InitializeCom()
{
	HRESULT hr = ::CoInitialize(NULL);
	if (hr != S_OK)
		return false;
	return true;
}

bool CanEasyHelper::LoadWorkSpace(QString WorkspacePath){ return false; }

bool CanEasyHelper::StartSimulation(){ return false; }
bool CanEasyHelper::StopSimulation(){ return false; }
#pragma endregion


void CanEasyHelper::ChangeWorkspace(){}
void CanEasyHelper::StopCanEasy(){}