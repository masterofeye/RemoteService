// Dies ist die Haupt-DLL.

#include "stdafx.h"

#using <DiagUtils.dll>
#using <FlasherUtils.dll>

#include "RemoteBoxWrapper.h"
#include <msclr\auto_gcroot.h>

using namespace System;
using namespace DiagUtils;
using namespace DiagUtils::IOBox;

namespace RemoteBoxWrapper {

    class WrapperCommand
    {
    public: 
        msclr::auto_gcroot<IOBoxUdsCommands ^> m_Commands;
    };

    Wrapper::Wrapper()
    {
        m_private = new WrapperCommand();
        m_private->m_Commands = gcnew IOBoxUdsCommands(gcnew DiagUtils::UDS::UdsServiceFactory(), DiagUtils::ISOTP::ISOTPDataExchange::Instance);
    }

    bool Wrapper::Init()
    {
        try{
            DiagUtils::UDS::IUdsDataExchange ^diag = DiagUtils::ISOTP::ISOTPDataExchange::Instance;

            auto timeout = diag->RequestTimeout;
            diag->RequestTimeout = 0;
            auto result = m_private->m_Commands->ResetDevice();
			diag->Deinit();
            diag->RequestTimeout = 500;
            result = diag->Init();
            return result.Success;
        }
        catch (...)
        {
            return false;
        }
        return true;
    }

    void Wrapper::DeInit()
    {
        DiagUtils::UDS::IUdsDataExchange ^diag = DiagUtils::ISOTP::ISOTPDataExchange::Instance;

        auto timeout = diag->RequestTimeout;
        diag->RequestTimeout = 0;
        auto result = m_private->m_Commands->ResetDevice();
        diag->Deinit();
        
        diag->RequestTimeout = timeout;
    }

	bool Wrapper::GetAdcValue(unsigned int adcId, short &value)
	{
		try{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetAdcValue(adcId, value);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			auto result = m_private->m_Commands->ResetDevice();

			DiagUtils::UDS::IUdsDataExchange ^diag = DiagUtils::ISOTP::ISOTPDataExchange::Instance;
			diag->Deinit();
			diag->Init();
			return false;
		}
    }

    bool Wrapper::GetDIOPinState(unsigned short pinId, bool &active)
    {
		try{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetDIOPinState(pinId, active);
				if (res.Success)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::GetFirmwareVersion(char* data)
    {
		try
		{
			array<unsigned char>^ local = gcnew array< unsigned char >(256);

			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetFirmwareVersion(local);
				if (res.Success)
				{
					pin_ptr<unsigned char> p = &local[0];
					unsigned char* pby = p;
					data = reinterpret_cast<char*>(pby);
				}
			}
			delete local;
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::GetHardwareSerialNumber(char* data)
    {
		try
		{
			array<unsigned char>^ local = gcnew array< unsigned char >(256);

			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetHardwareSerialNumber(local);
				if (res.Success)
				{
					pin_ptr<unsigned char> p = &local[0];
					unsigned char* pby = p;
					data = reinterpret_cast<char*>(pby);
				}
			}
			delete local;
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::GetPwmValue(unsigned char pwmId, unsigned short &value)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetPwmValue(pwmId, value);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::GetRelayState(unsigned short &state)
    {
		try{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetRelayState(state);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::GetUsbState(unsigned char hub, unsigned char &state)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetUsbState(hub, state);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool  Wrapper::ReadIOBoxConfig()
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				//res = m_Commands->ReadIOBoxConfig(config);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::ReadIOBoxId()
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				//res = m_Commands->ReadIOBoxId(id);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::ResetConfig()
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->ResetConfig();
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::ResetFirmwareChecksum()
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->ResetFirmwareChecksum();
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::SetDacValue(unsigned char dacId, unsigned short value)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->SetDacValue(dacId, value);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::SetDIOPinState(unsigned short pinId, bool active)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->SetDIOPinState(pinId, active);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::SetPwmValue(unsigned char pwmId, unsigned short value)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->SetPwmValue(pwmId, value);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::SetRelayState(unsigned short state)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->SetRelayState(state);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			auto result = m_private->m_Commands->ResetDevice();

			DiagUtils::UDS::IUdsDataExchange ^diag = DiagUtils::ISOTP::ISOTPDataExchange::Instance;
			diag->Deinit();
			diag->Init();
			return false;
		}
    }

    bool Wrapper::SetUsbState(unsigned char hub, unsigned char state)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->SetUsbState(hub, state);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::UploadConfig()
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->UploadConfig();
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::WriteIOBoxConfig()
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				//res = m_Commands->WriteIOBoxConfig(config);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }

    bool Wrapper::WriteIOBoxId()
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				//res = m_Commands->WriteIOBoxId(id);
				if (res.Success)
				{

				}
			}
			return res.Success;
		}
		catch (...)
		{
			return false;
		}
    }
}