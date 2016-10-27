// RemoteBoxWrapper.h

#pragma once

namespace RemoteBoxWrapper {

    class WrapperCommand;

    class _declspec(dllexport) Wrapper
	{
    private:
        WrapperCommand* m_private;
    public:
        Wrapper();

        bool Init(); 
        void DeInit();


        bool GetAdcValue(unsigned int adcId, short& value);
        bool GetDIOPinState(unsigned short pinId, bool&  active);
        bool GetFirmwareVersion( char* data);
        bool GetHardwareSerialNumber(char* data);
        bool GetPwmValue(unsigned char pwmId, unsigned short &value);
        bool GetRelayState( unsigned short &state);
        bool GetUsbState(unsigned char hub, unsigned char &state);
        bool ReadIOBoxConfig();
        bool ReadIOBoxId();
        bool ResetConfig();
        bool ResetFirmwareChecksum();
        bool SetDacValue(unsigned char dacId, unsigned short value);
        bool SetDIOPinState(unsigned short pinId, bool active);
        bool SetPwmValue(unsigned char pwmId, unsigned short value);
        bool SetRelayState(unsigned short state);
        bool SetUsbState(unsigned char hub, unsigned char state);
        bool UploadConfig();
        bool WriteIOBoxConfig();
        bool WriteIOBoxId();


		// TODO: Die Methoden für diese Klasse hier hinzufügen.
	};
}
