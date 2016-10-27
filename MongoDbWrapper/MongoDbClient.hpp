#pragma once

//#include <bson\bson.h>
//#include <Windows.h>

#include <QtCore>
#include "spdlog\spdlog.h"
#include "MongoDBWrapper_global.h"


namespace mongo
{
	class DBClientBase;
	class BSONObj;
}

namespace RW{
	namespace MONGO
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

		class MONGODBWRAPPER_EXPORT MongoDbClient : public QObject
		{
			Q_OBJECT
		private:
			static mongo::DBClientBase* m_Con;
			static std::shared_ptr<spdlog::logger> m_Logger;
			static MongoDbClient *m_Instance;
			static std::once_flag  m_once;

			//MongoDbClient(const std::string &Connection, QObject *Parent = nullptr);
		public:
			MongoDbClient();
			~MongoDbClient();

		public:
			static MongoDbClient* Instance(){
				//Workaround here, because QT object copy is private so we can't use static implementation
				std::call_once
					(
					MongoDbClient::m_once,
					[](){ m_Instance = new MongoDbClient(); }
				);
				return m_Instance;
			}

			static bool InitMongo(const std::string &Connection,std::shared_ptr<spdlog::logger> Logger);
			static bool CreateDatabase();


		public slots:
			static void UpdateData(const std::string Collection, mongo::BSONObj* Data);

			static void InsertData(CollectionType CollectionType, mongo::BSONObj *Data);
			static void InsertNewUser(mongo::BSONObj *Data);
			static void InsertNewRemoteWorkstation(mongo::BSONObj *Data);
			static void InsertNewSimulationPC(mongo::BSONObj *Data);
			static void InsertNewBackendPc(mongo::BSONObj *Data);
			static void InsertNewGeneralInformation(mongo::BSONObj *Data);
			static void InsertLog(BaseDbObject *Data);
			static BaseDbObject* GeneralInformation(QString Version);
			
		};
	}
}




