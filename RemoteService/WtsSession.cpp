#include "WtsSession.h"
#include <spdlog\spdlog.h>

#include "RemoteDataConnectLibrary.h"

#include <windows.h>
#include <Wtsapi32.h>
#include <userenv.h>

namespace RW
{
    namespace CORE
    {
		WtsSession::WtsSession(quint16 SessionId, QObject* Parent = nullptr) : QObject(Parent),
			m_Logger(spdlog::get("remoteservice")),
			m_SessionID(SessionId)
        {
        }


        WtsSession::~WtsSession()
        {
        }

		bool WtsSession::IsActive() const
		{
			DWORD bytesReturned(0);
			LPTSTR pBuf(NULL);
			if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, m_SessionID, WTSConnectState, &pBuf, &bytesReturned))
			{
				// this is not exceptional situation, the session may be closed at the moment, or close operation is pending on the session
				return false;
			}
			int connectState = *(reinterpret_cast<int*> (pBuf));
			return (connectState == WTSActive);
		}

		bool WtsSession::IsDisconnected() const
		{
			DWORD bytesReturned(0);
			LPTSTR pBuf(NULL);
			if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, m_SessionID, WTSConnectState, &pBuf, &bytesReturned))
			{
				// this is not exceptional situation, the session may be closed at the moment, or close operation is pending on the session
				return false;
			}
			int connectState = *(reinterpret_cast<int*> (pBuf));
			return (connectState == WTSDisconnected);
		}

		double WtsSession::GetLogonTime() const
		{
			DWORD bytesReturned(0);
			LPTSTR pBuf(NULL);
			if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, m_SessionID, WTSLogonTime, &pBuf, &bytesReturned))
			{
				throw std::runtime_error("GotLogonTime session time failed");
			}
			return (*(reinterpret_cast<double*> (pBuf)));
		}

		QString WtsSession::GetSessionUserName() const
		{
			DWORD bytesReturned(0);
			LPTSTR pBuf(NULL);
			if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, m_SessionID, WTSUserName, &pBuf, &bytesReturned))
			{
				m_Logger->error("GetSessionUserName failed. ErrorID: {}", (int)spdlog::sinks::FilterType::WtsSession, GetLastError());
			}
			return QString::fromWCharArray(pBuf);
		}

		QString WtsSession::GetDomainName() const
		{
			DWORD bytesReturned(0);
			LPTSTR pBuf(NULL);
			if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, m_SessionID, WTSDomainName, &pBuf, &bytesReturned))
			{
				m_Logger->error("GetDomainName failed. ErrorID: {}", (int)spdlog::sinks::FilterType::WtsSession, GetLastError());
			}
			return  QString::fromWCharArray(pBuf);
		}
    }
}