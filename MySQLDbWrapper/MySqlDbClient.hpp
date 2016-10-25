#pragma once

//#include <bson\bson.h>
//#include <Windows.h>

#include <QtCore>



#include <mutex>

/*Own Include Files*/
#include "spdlog\spdlog.h"
#include "MySqlDbWrapper_global.h"

	
/******************************************************************************************************************
@autor Ivo Kunadt
@brief 
********************************************************************************************************************/
namespace MYSQL
{
	class DBClientBase;
	class BSONObj;
}

namespace sql{
	class Driver;
	class Connection;
}

namespace RW{
	namespace MYSQL
	{
		class BaseDbObject;
		enum class CollectionType
		{
			nenUser,
			nenHistory,
			nenRemoteWorkstation,
			nenBackendPC,
			nenSimulationPC,
			nenGeneral

		};

		const std::string DATABASE_NAME = "remoteWorkstation";
		const std::string USER_COLLECTION = "users";
		const std::string REMOTE_COLLECTION = "remoteWorkstations";
		const std::string SIMULATION_COLLECTION = "simulationWorkstations";
		const std::string BACKEND_COLLECTION = "backendWorkstations";
		const std::string GERNERAL_COLLECTION = "generalInformation";
		const std::string HISTORY_COLLECTION = "flashhistory";
		const std::string LOGS = "logs";

		class MYSQLDBWRAPPER_EXPORT MySqlDbClient : public QObject
		{
			Q_OBJECT
		private:
			static sql::Driver *m_Driver;
			static std::unique_ptr<sql::Connection> m_Con;
			static MySqlDbClient *m_Instance;
			static std::once_flag  m_once;
			static std::shared_ptr<spdlog::logger> m_Logger;

			//MongoDbClient(const std::string &Connection, QObject *Parent = nullptr);
		public:
			MySqlDbClient();
			~MySqlDbClient();

		public:
			static MySqlDbClient* Instance(){
				//Workaround here, because QT object copy is private so we can't use static implementation
				std::call_once
					(
					MySqlDbClient::m_once,
					[](){ m_Instance = new MySqlDbClient(); }
				);
				return m_Instance;
			}
			
			static bool InitMySQL(std::shared_ptr<spdlog::logger> Logger, const std::string URL = "tcp://127.0.0.1:3306", const std::string &User = "root", const std::string &Password = "root");
			static bool CreateDatabase();


		public slots:
			static void UpdateData(const std::string Collection);

			static void InsertData(CollectionType CollectionType);
			static void InsertNewUser();
			static void InsertNewRemoteWorkstation();
			static void InsertNewSimulationPC();
			static void InsertNewBackendPc();
			static void InsertNewGeneralInformation();
			static void InsertLog(BaseDbObject *Data);
			static BaseDbObject* GeneralInformation(QString Version);
			
		};
	}
}




