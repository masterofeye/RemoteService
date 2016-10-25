#include "BasicServer.h"

namespace RW{
	namespace CORE{
		BasicServer::BasicServer(QObject *Parent = nullptr) : QObject(Parent),
			m_logger(spdlog::get("file_logger"))
		{
		}


		BasicServer::~BasicServer()
		{
		}
	}
}