#pragma once

#include <qobject.h>
namespace RW{
    namespace CORE{
#ifdef QT
        class Util
        {
            Q_GADGET
        public:
#endif
            enum class FlashType
            {
                Bootloader,
                AC,
                GC,
                NON,
            };
#ifdef QT
            Q_ENUM(FlashType)
#endif
            enum class MessageReceiver
            {
                CommunicationServer,
                ProcessManager,
                CanEasyWrapper,
                BasicWrapper,
                PortalInfo,
                FHostSPWrapper,
                MKSWrapper,
                FileUtil,
                UsbHidLoader,
				InactivityWatcher,
				Amount
            };
#ifdef QT
            Q_ENUM(MessageReceiver)
#endif
            enum class Functions
            {
                EX_CanEasyStartApplication,
				EX_CanEasyLoadWorkspace,
				EX_CanEasyCloseApplication,
				EX_CanEasyStartSimulation,
				EX_CanEasyStopSimulation,
				EX_MKSLogin,
				EX_MKSStartDownload,
                EX_MKSCreateSandBox,
                EX_MKSDropSandbox,
                EX_MKSClose,
                EX_FHostSPStartFHost,
                EX_FHostSPLoadFlashFile,
                EX_FHostSPCloseFHost,
                EX_FHostSPGetState,
                EX_FHostSPAbortSequence,
                EX_FHostSPCloseSequence,
                EX_FHostSPStartFlash,
                EX_FHostSPGetProgress,
                EX_PortalInfoFittingAC,
                EX_PortalInfoAcByIcom,
                EX_PortalInfoReleases,
                EX_PortalInfoSoftwareById,
                EX_PortalInfoShowDialog,
                EX_PortalInfoCloseDialog,
                EX_FileUtilUnZip,
                EX_FileUtilDelete,
                EX_UsbHidLoaderFlashFile,
                EX_PrintDebugInformation,
				IN_ToggleCl30Fast,
				IN_ToggleCl30Slow,
				EX_StartInactivityTimer,
				EX_StopInactivityTimer,
				IN_PowerSupply,
				IN_PowerStripe,
                Amount
            };
#ifdef QT
            Q_ENUM(Functions)
#endif
            enum class ErrorID
            {
                Success,
                Error,
                Busy,
                ErrorCanEasyComServerMissing,
                ErrorCanEasyStartSimulation,
                ErrorCanEasySimulationIsRunningFailed,
                ErrorCanEasyStopSimulation,
                ErrorCanEasyWorkspaceNotFound,
                ErrorCanEasyWorkspaceNotLoaded,
                ErrorCanEasyApplicationError,
                ErrorCanEasyDeInitError,
                ErrorMKSLogin,
                ErrorMKSLocationMissing,
                ErrorMKSError,
                ErrorMKSSandBoxCreation,
                ErrorMKSSanbBoxDrop,
                ErrorMKSCloseFailed,
                ErrorMKSMissingParameter,
                ErrorFHostSPStartApplication,
                ErrorFHostSPSequenceStop,
                ErrorFHostSPSequenceStart,
                ErrorFHostSPLoadFlashfileStatusFailed,
                ErrorFHostSPLoadFlashfileFailed,
                ErrorFHostSPFlashfileNotExits,
                ErrorFHostSPGetProgress,
                ErrorFHostSPCloseApplication,
                ErrorFHostSPGetStateFailed,
                ErrorFHostSPAbortFailed,
                ErrorPortalInfoFinalRegexCheck,
                ErrorPortalInfoProjectnameCount,
                ErrorPortalInfoProjectCount,
                ErrorPortalInfoSamplePhaseAndReleaseCount,
                ErrorPortalInfoSamplePhaseCount,
                ErrorPortalInfoPrepareReleaseInformation,
                ErrorPortalInfoReleaseCount,
                ErrorFileUtilMissingZipFile,
                ErrorFileUtilNoSevenZip,
                ErrorFileUtilFailed,
                ErrorFileUtilDirectoryDontExists,
                ErrorFileUtilDeleteFailed,
                ErrorFileUsbHidLoaderFileDontExists,
                ErrorFileUsbHidLoaderFailed,
                ErrorFileUsbHidLoaderHIDState,
                ErrorFileUsbHidLoaderTimeOut,
                ErrorFileUsbHidLoaderChecksum,
                ErrorFileUsbHidLiaderUnknownError,
				Amount
            };
#ifdef QT
            Q_ENUM(ErrorID)
#endif

            enum class FHostSPState
            {
                JOB_EXECUTE_OK,
                JOB_EXECUTE_NOT_OK,
                JOB_IDLE,
                JOB_LOADING,
                JOB_LOAD_NOT_OK,
                JOB_LOAD_OK,
                JOB_LOAD_WARNING,
                JOB_EXECUTING,
                JOB_NO_DONGLE
            };
#ifdef QT
            Q_ENUM(FHostSPState)
        };
#endif



        typedef struct _FlashInfo
        {
            QString FlashFile;
            Util::FlashType FlashType;
            QString TargetDir;
            bool IsUsb;

            _FlashInfo()
            {
                FlashFile = "";
                FlashType = Util::FlashType::NON;
                TargetDir = "";
                IsUsb = false;
            }
            _FlashInfo(QString FlashFile, Util::FlashType, QString TargetDir, bool IsUsb)
            {
                this->FlashFile = FlashFile;
                this->FlashType = FlashType;
                this->TargetDir = TargetDir;
                this->IsUsb = IsUsb;
            }
        }FlashInfo;
    }
}