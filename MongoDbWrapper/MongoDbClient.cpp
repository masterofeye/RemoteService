#include "MongoDbClient.hpp"
#include "BaseDbObject.h"
#include <Winsock2.h>
#include <mongo\client\dbclient.h>


#include "GeneralObject.hpp"
#include "HistoryObject.hpp"
#include "RemoteWorkstationObject.hpp"
#include "UserObject.hpp"
#include "SimulationObject.hpp"
#include "LogObject.h"

using namespace std::chrono;

namespace RW{
	namespace MONGO
	{
		MongoDbClient* MongoDbClient::m_Instance = nullptr;
		std::once_flag MongoDbClient::m_once;
		mongo::DBClientBase* MongoDbClient::m_Con = nullptr;
		std::shared_ptr<spdlog::logger> MongoDbClient::m_Logger;

		MongoDbClient::MongoDbClient()
		{
		}

		MongoDbClient::~MongoDbClient()
		{
			m_Logger->debug("MongoDbClient destroyed");
		}

		bool MongoDbClient::InitMongo(const std::string &Connection, std::shared_ptr<spdlog::logger> Logger){
			try{
				m_Logger = Logger;

				mongo::Status stat = mongo::client::initialize();
				if (stat.isOK())
				{
					std::string err_msg; 
					mongo::ConnectionString cs = mongo::ConnectionString::parse(Connection, err_msg);

					if (!cs.isValid()) {
						//m_logger->error("MongoDbClient couldn't initialized.");
						return false;
					}

					m_Con = cs.connect(err_msg);
					if (!m_Con) {
						//m_logger->error("MongoDbClient couldn't connected.");
						return false;
					}
					else
					{
						//m_logger->debug("MongoDbClient connected.");
						return true;
					}
				}
				else
				{
					//m_logger->error("MongoDbClient couldn't initialized");
					return false;
				}
			}
			catch (const mongo::DBException &e)
			{
				return false;
			}
		}


		bool MongoDbClient::CreateDatabase()
		{
			m_Con->dropDatabase(DATABASE_NAME);
			
			m_Con->createCollection(DATABASE_NAME + "." + LOGS);
			//mongo::BSONObj objLog = BSON("time" << mongo::dateFromISOString("2000-10-10T20:55:36Z").getValue() << "type" << "" << "logger" << "debug" << "thread" << "thread 4616" << "message" << "This is a test message.");
			//m_Con->insert(DATABASE_NAME + "." + LOGS, objLog);

			//for (int i = 0; i < 2000; i++)
			//{
			//	m_Con->insert(DATABASE_NAME + "." + LOGS, objLog);
			//}

			m_Con->createCollection(DATABASE_NAME + "." + USER_COLLECTION);
			mongo::BSONObj objUserAdmin = BSON("user" << "kunadt" << "password" << "schleissheimer" << "role" << "admin");
			m_Con->insert(DATABASE_NAME + "." + USER_COLLECTION, objUserAdmin);

			mongo::BSONObj objUserKay = BSON("user" << "müller" << "password" << "1234" << "role" << "default");
			m_Con->insert(DATABASE_NAME + "." + USER_COLLECTION, objUserKay);


			mongo::BSONObj userKunadt = m_Con->findOne(DATABASE_NAME + "." + USER_COLLECTION, BSON("user" << "kunadt"));
			mongo::BSONElement idKunadt;
			userKunadt.getObjectID(idKunadt);



			

			/*************RemoteWorkstation************************/
			/*************W213 START************************/
			mongo::BSONObj objRemote4 = BSON("project" << "Daimler W213 MY16 HL" \
				<< "a_number" << "A862" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.13" << "pc_mac" << "34:97:F6:9B:3A:9E" << "powerstripe_ip" << "192.168.111.85")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 0)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb"<< "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Debug/Flash" << "name" << "Flash" << "type" << "flash" << "flash" << true << "default" << "off" << "pin" << 8) \
				<< BSON("group" << "Peripherie" << "name" << "PowerSupply" << "type" << "power" << "flash" << false << "default" << "off" << "pin" << 1) \
				<< BSON("group" << "Debug/Flash" << "name" << "FHostSP" << "type" << "fhost" << "flash" << true << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Audio" << "name" << "Audio" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Debug/Flash" << "name" << "AutoFlash" << "type" << "auto" << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Light" << "name" << "Light" << "type" << "relay" << "flash" << false << "default" << "off" << "pin" << 6) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote4);

			mongo::BSONObj pcA862 = m_Con->findOne(DATABASE_NAME + "." + REMOTE_COLLECTION, BSON("a_number" << "A862"));
			mongo::BSONElement idPcA862;
			pcA862.getObjectID(idPcA862);


			mongo::BSONObj objRemote5 = BSON("project" << "Daimler W213 MY16 HL" \
				<< "a_number" << "A822" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.14" << "pc_mac" << "9C:5C:8E:90:D3:99" << "powerstripe_ip" << "192.168.111.86")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 0)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Debug/Flash" << "name" << "Flash" << "type" << "flash" << "flash" << true << "default" << "off" << "pin" << 8) \
				<< BSON("group" << "Peripherie" << "name" << "PowerSupply" << "type" << "power" << "flash" << false << "default" << "off" << "pin" << 1) \
				<< BSON("group" << "Debug/Flash" << "name" << "FHostSP" << "type" << "fhost" << "flash" << true << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Audio" << "name" << "Audio" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Debug/Flash" << "name" << "AutoFlash" << "type" << "auto" << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Light" << "name" << "Light" << "type" << "relay" << "flash" << false << "default" << "off" << "pin" << 6) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote5);
			/*************W213 END************************/




			m_Con->createCollection(DATABASE_NAME + "." + REMOTE_COLLECTION);
			mongo::BSONObj objRemote = BSON("project" << "Daimler W222 Mopf MY17 HL" \
				<< "a_number" << "A684" \
				<< "state" << "ON" \
				<< "user_id" << idKunadt \
				<< "hw_version" << "1.01" \
				<< "sw_version" << "0.2" \
				<< "hw" << BSON("pc_ip" << "192.168.0.10" << "pc_mac" << "aa:bb:cc:dd:ee" << "powerstripe_ip" << "192.168.0.11")\
				<< "permanentlog" << BSON("pl" << true << "reason" << "WDM Analisys")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << "" \
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
										<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
										<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
								));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote);

			mongo::BSONObj RemoteAP_A684 = m_Con->findOne(DATABASE_NAME + "." + REMOTE_COLLECTION, BSON("a_number" << "A684"));
			if (!RemoteAP_A684.isEmpty())
			{
				mongo::BSONElement RemoteAP_A684_Element;
				RemoteAP_A684.getObjectID(RemoteAP_A684_Element);
			}

			mongo::BSONObj objRemote_A717 = BSON("project" << "Daimler W222 Mopf MY17 HL" \
				<< "a_number" << "A717" \
				<< "state" << "OFFLINE" \
				<< "user_id" << idKunadt\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.11" << "pc_mac" << "A0:D3:C1:4F:46:2A" << "powerstripe_ip" << "192.168.111.84")\
				<< "permanentlog" << BSON("pl" << true << "reason" << "WDM Analisys")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
										<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
										<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A717);


			mongo::BSONObj objRemote_A821 = BSON("project" << "Daimler W222 Mopf MY17 HL" \
				<< "a_number" << "A821" \
				<< "state" << "OFFLINE" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.12" << "pc_mac" << "9C:5C:8E:90:D4:47" << "powerstripe_ip" << "192.168.111.85")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
										<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
										<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A821);

			mongo::BSONObj objRemote_A823 = BSON("project" << "Daimler W222 Mopf MY17 HL" \
				<< "a_number" << "A823" \
				<< "state" << "OFFLINE" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.12" << "pc_mac" << "9C:5C:8E:90:D4:98" << "powerstripe_ip" << "192.168.111.80")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 0)\
										<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
										<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
										<< BSON("group" << "Debug/Flash" << "name" << "Flash" << "type" << "flash" << "flash" << true << "default" << "off" << "pin" << 8) \
										<< BSON("group" << "Peripherie" << "name" << "PowerSupply" << "type" << "power" << "flash" << false << "default" << "off" << "pin" << 1) \
										<< BSON("group" << "Debug/Flash" << "name" << "FHostSP" << "type" << "fhost" << "flash" << true << "default" << "off" << "pin" << 5) \
										<< BSON("group" << "Audio" << "name" << "Audio" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 5) \
										<< BSON("group" << "Debug/Flash" << "name" << "AutoFlash" << "type" << "auto" << "flash" << false << "default" << "off" << "pin" << 0) \
										<< BSON("group" << "Light" << "name" << "Light" << "type" << "relay" << "flash" << false << "default" << "off" << "pin" << 6) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A823);

			mongo::BSONObj objRemote_A795 = BSON("project" << "Daimler W222 Mopf MY17 HL" \
				<< "a_number" << "A795" \
				<< "state" << "OFFLINE" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.12" << "pc_mac" << "9C:5C:8E:8F:AB:40" << "powerstripe_ip" << "192.168.111.85")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Debug/Flash" << "name" << "Flash" << "type" << "flash" << "flash" << true << "default" << "off" << "pin" << 8) \
				<< BSON("group" << "Peripherie" << "name" << "PowerSupply" << "type" << "power" << "flash" << false << "default" << "off" << "pin" << 1) \
				<< BSON("group" << "Debug/Flash" << "name" << "FHostSP" << "type" << "fhost" << "flash" << true << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Audio" << "name" << "Audio" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Debug/Flash" << "name" << "AutoFlash" << "type" << "auto" << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Light" << "name" << "Light" << "type" << "relay" << "flash" << false << "default" << "off" << "pin" << 6) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A795);
			/*************W222 Mopf END************************/

			/*************W213/W222 Mopf EL START************************/
			mongo::BSONObj objRemote_A772 = BSON("project" << "Daimler MY16/17 W213/W222 Mopf EL" \
				<< "a_number" << "A772" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.15" << "pc_mac" << "2C:56:DC:9A:5D:DF" << "powerstripe_ip" << "192.168.111.87" << "hw_version" << 1.01 << "sw_version" << 0.2)\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A772);

			mongo::BSONObj objRemote_A796 = BSON("project" << "Daimler MY16/17 W213/W222 Mopf EL" \
				<< "a_number" << "A796" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.15" << "pc_mac" << "9C:5C:8E:8F:AB:2D" << "powerstripe_ip" << "192.168.111.87" << "hw_version" << 1.01 << "sw_version" << 0.2)\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A796);

			mongo::BSONObj objRemote_A855 = BSON("project" << "Daimler MY16/17 W213/W222 Mopf EL" \
				<< "a_number" << "A855" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.15" << "pc_mac" << "D0:17:C2:97:04:1F" << "powerstripe_ip" << "192.168.111.87" << "hw_version" << 1.01 << "sw_version" << 0.2)\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A855);
			/*************W205 Mopf EL END************************/


			/*************W205 Mopf HL MY18 START************************/
			mongo::BSONObj objRemote_A843 = BSON("project" << "Daimler MY18 W205 Mopf HL" \
				<< "a_number" << "A843" \
				<< "state" << "OFFLINE" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.13" << "pc_mac" << "D0:17:C2:97:07:2E" << "powerstripe_ip" << "192.168.111.85")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 0)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Debug/Flash" << "name" << "Flash" << "type" << "flash" << "flash" << true << "default" << "off" << "pin" << 8) \
				<< BSON("group" << "Peripherie" << "name" << "PowerSupply" << "type" << "power" << "flash" << false << "default" << "off" << "pin" << 1) \
				<< BSON("group" << "Debug/Flash" << "name" << "FHostSP" << "type" << "fhost" << "flash" << true << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Audio" << "name" << "Audio" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Debug/Flash" << "name" << "AutoFlash" << "type" << "auto" << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Light" << "name" << "Light" << "type" << "relay" << "flash" << false << "default" << "off" << "pin" << 6) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A843);

			mongo::BSONObj objRemote_A856 = BSON("project" << "Daimler MY18 W205 Mopf HL" \
				<< "a_number" << "A856" \
				<< "state" << "OFFLINE" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.14" << "pc_mac" << "D0:17:C2:9A:00:83" << "powerstripe_ip" << "192.168.111.86")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 0)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Debug/Flash" << "name" << "Flash" << "type" << "flash" << "flash" << true << "default" << "off" << "pin" << 8) \
				<< BSON("group" << "Peripherie" << "name" << "PowerSupply" << "type" << "power" << "flash" << false << "default" << "off" << "pin" << 1) \
				<< BSON("group" << "Debug/Flash" << "name" << "FHostSP" << "type" << "fhost" << "flash" << true << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Audio" << "name" << "Audio" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Debug/Flash" << "name" << "AutoFlash" << "type" << "auto" << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Light" << "name" << "Light" << "type" << "relay" << "flash" << false << "default" << "off" << "pin" << 6) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A856);


			mongo::BSONObj objRemote_A864 = BSON("project" << "Daimler MY18 W205 Mopf HL" \
				<< "a_number" << "A864" \
				<< "state" << "OFFLINE" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.14" << "pc_mac" << "34:97:F6:9B:3B:FA" << "powerstripe_ip" << "192.168.111.86")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 0)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Debug/Flash" << "name" << "Flash" << "type" << "flash" << "flash" << true << "default" << "off" << "pin" << 8) \
				<< BSON("group" << "Peripherie" << "name" << "PowerSupply" << "type" << "power" << "flash" << false << "default" << "off" << "pin" << 1) \
				<< BSON("group" << "Debug/Flash" << "name" << "FHostSP" << "type" << "fhost" << "flash" << true << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Audio" << "name" << "Audio" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Debug/Flash" << "name" << "AutoFlash" << "type" << "auto" << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Light" << "name" << "Light" << "type" << "relay" << "flash" << false << "default" << "off" << "pin" << 6) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A864);

			mongo::BSONObj objRemote_A860 = BSON("project" << "Daimler MY18 W205 Mopf HL" \
				<< "a_number" << "A860" \
				<< "state" << "OFFLINE" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.14" << "pc_mac" << "D0:17:C2:97:03:E3" << "powerstripe_ip" << "192.168.111.86")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 0)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Debug/Flash" << "name" << "Flash" << "type" << "flash" << "flash" << true << "default" << "off" << "pin" << 8) \
				<< BSON("group" << "Peripherie" << "name" << "PowerSupply" << "type" << "power" << "flash" << false << "default" << "off" << "pin" << 1) \
				<< BSON("group" << "Debug/Flash" << "name" << "FHostSP" << "type" << "fhost" << "flash" << true << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Audio" << "name" << "Audio" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Debug/Flash" << "name" << "AutoFlash" << "type" << "auto" << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Light" << "name" << "Light" << "type" << "relay" << "flash" << false << "default" << "off" << "pin" << 6) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A860);

			mongo::BSONObj objRemote_A859 = BSON("project" << "Daimler MY18 W205 Mopf HL" \
				<< "a_number" << "A859" \
				<< "state" << "OFFLINE" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.14" << "pc_mac" << "D0:17:C2:9A:61:27" << "powerstripe_ip" << "192.168.111.86")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 0)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Debug/Flash" << "name" << "Flash" << "type" << "flash" << "flash" << true << "default" << "off" << "pin" << 8) \
				<< BSON("group" << "Peripherie" << "name" << "PowerSupply" << "type" << "power" << "flash" << false << "default" << "off" << "pin" << 1) \
				<< BSON("group" << "Debug/Flash" << "name" << "FHostSP" << "type" << "fhost" << "flash" << true << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Audio" << "name" << "Audio" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Debug/Flash" << "name" << "AutoFlash" << "type" << "auto" << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Light" << "name" << "Light" << "type" << "relay" << "flash" << false << "default" << "off" << "pin" << 6) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A859);

			mongo::BSONObj objRemote_A858 = BSON("project" << "Daimler MY18 W205 Mopf HL" \
				<< "a_number" << "A858" \
				<< "state" << "OFFLINE" \
				<< "user_id" << ""\
				<< "hw_version" << 1.01 \
				<< "sw_version" << 0.2 \
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.14" << "pc_mac" << "D0:17:C2:97:05:25" << "powerstripe_ip" << "192.168.111.86")\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 0)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Debug/Flash" << "name" << "Flash" << "type" << "flash" << "flash" << true << "default" << "off" << "pin" << 8) \
				<< BSON("group" << "Peripherie" << "name" << "PowerSupply" << "type" << "power" << "flash" << false << "default" << "off" << "pin" << 1) \
				<< BSON("group" << "Debug/Flash" << "name" << "FHostSP" << "type" << "fhost" << "flash" << true << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Audio" << "name" << "Audio" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 5) \
				<< BSON("group" << "Debug/Flash" << "name" << "AutoFlash" << "type" << "auto" << "flash" << false << "default" << "off" << "pin" << 0) \
				<< BSON("group" << "Light" << "name" << "Light" << "type" << "relay" << "flash" << false << "default" << "off" << "pin" << 6) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A858);

			/*************W205 MOPF HL END************************/

			/*************W205 Mopf EL START************************/
			mongo::BSONObj objRemote_A798 = BSON("project" << "Daimler MY18 W205 Mopf EL" \
				<< "a_number" << "A798" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.15" << "pc_mac" << "9C:5C:8E:8F:AB:8A" << "powerstripe_ip" << "192.168.111.87" << "hw_version" << 1.01 << "sw_version" << 0.2)\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A798);

			mongo::BSONObj objRemote_A841 = BSON("project" << "Daimler MY18 W205 Mopf EL" \
				<< "a_number" << "A841" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.15" << "pc_mac" << "9C:5C:8E:90:D4:52" << "powerstripe_ip" << "192.168.111.87" << "hw_version" << 1.01 << "sw_version" << 0.2)\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A841);
			/*************W205 Mopf EL END************************/

			/*************VS30-HL Start************************/
			mongo::BSONObj objRemote_A797 = BSON("project" << "VS30-HL" \
				<< "a_number" << "A797" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.15" << "pc_mac" << "9C:5C:8E:8F:AA:09" << "powerstripe_ip" << "192.168.111.87" << "hw_version" << 1.01 << "sw_version" << 0.2)\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A797);

			/*mongo::BSONObj objRemote_A841 = BSON("project" << "VS30-HL" \
				<< "a_number" << "A841" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.15" << "pc_mac" << "9C:5C:8E:90:D4:52" << "powerstripe_ip" << "192.168.111.87" << "hw_version" << 1.01 << "sw_version" << 0.2)\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb1" << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A841);*/
			/*************VS30-HL END************************/

			/*************VS30-LL Start************************/
			mongo::BSONObj objRemote_A799 = BSON("project" << "VS30-LL" \
				<< "a_number" << "A799" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.15" << "pc_mac" << "9C:5C:8E:8F:AB:29" << "powerstripe_ip" << "192.168.111.87" << "hw_version" << 1.01 << "sw_version" << 0.2)\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A799);

			mongo::BSONObj objRemote_A842 = BSON("project" << "VS30-LL" \
				<< "a_number" << "A842" \
				<< "state" << "Defect" \
				<< "user_id" << ""\
				<< "remoteapp_version" << 0.4 \
				<< "remoteservice_version" << 1.2 \
				<< "hw" << BSON("pc_ip" << "192.168.0.15" << "pc_mac" << "D0:17:C2:97:07:62" << "powerstripe_ip" << "192.168.111.87" << "hw_version" << 1.01 << "sw_version" << 0.2)\
				<< "permanentlog" << BSON("pl" << false << "reason" << "")\
				<< "features" << BSON_ARRAY("HUD" << "LVDS PIP") \
				<< "history_id" << ""\
				<< "buttons" << BSON_ARRAY(BSON("group" << "Devices" << "name" << "CAN" << "type" << "usb" << "port" << 0 << "flash" << false << "default" << "off" << "pin" << 1)\
				<< BSON("group" << "InstrumentCLuster" << "name" << "Kl30" << "type" << "io" << "flash" << false << "default" << "off" << "pin" << 2) \
				<< BSON("group" << "Debug/Flash" << "name" << "MiniCube" << "type" << "usb" << "port" << 1 << "flash" << false << "default" << "off" << "pin" << 0) \
				));
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, objRemote_A842);
			/*************VS30-LL END************************/




			/****************History************************/
			milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			

			m_Con->createCollection(DATABASE_NAME + "." + HISTORY_COLLECTION);
			mongo::BSONObj objRemote_History0 = BSON("rw_pc" << idPcA862 \
				<< "user" << idKunadt \
				<< "time" << ms.count()\
				<< "ac" << "AC_E015_Pre21" \
				<< "gc" << "GC_E015_Pre21" \
				<< "bootloader" << "Bootloader_Pre21");
			m_Con->insert(DATABASE_NAME + "." + HISTORY_COLLECTION, objRemote_History0);

			mongo::BSONObj objRemote_History1 = BSON("rw_pc" << idPcA862 \
				<< "user" << idKunadt \
				<< "time" << ms.count() \
				<< "ac" << "AC_E015_Pre22" \
				<< "gc" << "GC_E015_Pre22" \
				<< "bootloader" << "Bootloader_Pre22");
			m_Con->insert(DATABASE_NAME + "." + HISTORY_COLLECTION, objRemote_History1);

			mongo::BSONObj objRemote_History2 = BSON("rw_pc" << idPcA862 \
				<< "user" << idKunadt \
				<< "time" << ms.count() \
				<< "ac" << "AC_E015_Pre23" \
				<< "gc" << "GC_E015_Pre23" \
				<< "bootloader" << "Bootloader_Pre23");
			m_Con->insert(DATABASE_NAME + "." + HISTORY_COLLECTION, objRemote_History2);




			/****************Simulation************************/
			m_Con->createCollection(DATABASE_NAME + "." + SIMULATION_COLLECTION);
			mongo::BSONObj objRemote9 = BSON("a_number" << "A773" \
				<< "user_id" << idKunadt \
				<< "hw" << BSON("pc_ip" << "192.168.0.10" << "pc_mac" << "aa:bb:cc:dd:ee")\
				<< "permanentlog" << BSON("pl" << true << "reason" << "WDM Analisys")\
				<< "history_id" << "");
			m_Con->insert(DATABASE_NAME + "." + SIMULATION_COLLECTION, objRemote9);

			/****************Backend************************/
			m_Con->createCollection(DATABASE_NAME + "." + BACKEND_COLLECTION);
			mongo::BSONObj objRemote8 = BSON("a_number" << "A671" \
				<< "user_id" << idKunadt \
				<< "hw" << BSON("pc_ip" << "192.168.0.10" << "pc_mac" << "aa:bb:cc:dd:ee") \
				<< "permanentlog" << BSON("pl" << true << "reason" << "WDM Analisys") \
				<< "vm_stations" << BSON_ARRAY("A671-1" << "A671-2" << "A671-3" ) \
				<< "history_id" << "");
			m_Con->insert(DATABASE_NAME + "." + BACKEND_COLLECTION, objRemote8);

			mongo::BSONObj objRemote7 = BSON("a_number" << "A778" \
				<< "user_id" << idKunadt \
				<< "hw" << BSON("pc_ip" << "192.168.0.10" << "pc_mac" << "aa:bb:cc:dd:ee")\
				<< "permanentlog" << BSON("pl" << true << "reason" << "WDM Analisys")\
				<< "vm_stations" << BSON_ARRAY("A778-1" << "A778-2" << "A778-3" << "A778-4" << "A778-5" << "A778-6") \
				<< "history_id" << "");
			m_Con->insert(DATABASE_NAME + "." + BACKEND_COLLECTION, objRemote7);

			/****************General************************/
			m_Con->createCollection(DATABASE_NAME + "." + GERNERAL_COLLECTION);

			mongo::BSONObj obj = BSON("version"<< 0.1 <<"logoutTimeout" << 7200000 << "shutdownTimeout" << 1800000);
			m_Con->insert(DATABASE_NAME + "." + GERNERAL_COLLECTION, obj);

			/****************History************************/
			m_Con->createCollection(DATABASE_NAME + "." + HISTORY_COLLECTION);
			return true;
		}

		void MongoDbClient::InsertData(CollectionType CollectionType, mongo::BSONObj *Data)
		{
			switch (CollectionType)
			{
			case CollectionType::nenHistory:
				break;
			case CollectionType::nenUser:
				InsertNewUser(Data);
				break;
			case CollectionType::nenRemoteWorkstation:
				InsertNewRemoteWorkstation(Data);
				break;
			case CollectionType::nenSimulationPC:
				InsertNewSimulationPC(Data);
				break;
			case CollectionType::nenBackendPC: 
				InsertNewBackendPc(Data);
				break;
			case CollectionType::nenGeneral:
				InsertNewGeneralInformation(Data);
				break;
			default:
				m_Logger->alert("InsertData: No CollectionType found.");
				break;
			}
		}

		void MongoDbClient::InsertNewRemoteWorkstation(mongo::BSONObj *Data)
		{
			m_Con->insert(DATABASE_NAME + "." + REMOTE_COLLECTION, *Data);
			std::string error = m_Con->getLastError();
			if (error != "")
			{
				//TODO Error
			}
		}

		void MongoDbClient::InsertNewUser(mongo::BSONObj *Data)
		{
			m_Con->insert(DATABASE_NAME + "." + USER_COLLECTION, *Data);

			std::string error = m_Con->getLastError();
			if (error != "")
			{
				//TODO Error
			}
		}

		void MongoDbClient::InsertNewGeneralInformation(mongo::BSONObj *Data)
		{
			m_Con->insert(DATABASE_NAME + "." + GERNERAL_COLLECTION, *Data);

			std::string error = m_Con->getLastError();
			if (error != "")
			{
				//TODO Error
			}
		}

		void MongoDbClient::InsertNewBackendPc(mongo::BSONObj *Data)
		{
			m_Con->insert(DATABASE_NAME + "." + BACKEND_COLLECTION, *Data);

			std::string error = m_Con->getLastError();
			if (error != "")
			{
				//TODO Error
			}
		}

		void MongoDbClient::InsertNewSimulationPC(mongo::BSONObj *Data)
		{
			m_Con->insert(DATABASE_NAME + "." + SIMULATION_COLLECTION, *Data);

			std::string error = m_Con->getLastError();
			if (error != "")
			{
				//TODO Error
			}
		}

		void  MongoDbClient::InsertLog(BaseDbObject *Data)
		{
			LogObject *m_Object = (LogObject*)Data;
			mongo::Date_t t(m_Object->Date());
			quint64 time = t.millis;
			mongo::BSONObjBuilder b;

			b.appendNumber("time", m_Object->Date());
			

			mongo::BSONObj objLog = BSON("type" << m_Object->Type().toStdString() << "loggerLvl" << m_Object->LogLevel().toStdString() << "thread" << m_Object->ThreadId()<< "errorID" << m_Object->ErrorID() << "message" << m_Object->Message().toStdString() << "computer" << m_Object->ComputerName().toStdString());
			b.appendElements(objLog);
			m_Con->insert(DATABASE_NAME + "." + LOGS, b.obj());

			std::string error = m_Con->getLastError();
			if (error != "")
			{
				//TODO Error
			}
		}


		void MongoDbClient::UpdateData(const std::string Collection, mongo::BSONObj* Data)
		{
			if (Collection == "history")
			{

			}

			mongo::Query qr = MONGO_QUERY("" << 2);
			m_Con->update(Collection, qr, *Data);
			std::string error = m_Con->getLastError();
			if (error != "")
			{
				//TODO Error
			}
		}


		BaseDbObject* MongoDbClient::GeneralInformation(QString Version)
		{
			GeneralObject* base = nullptr;
			if (m_Con->isStillConnected())
			{

				mongo::BSONObj obj = m_Con->findOne(DATABASE_NAME + "." + GERNERAL_COLLECTION, MONGO_QUERY("version" << Version.toStdString()));
				if (obj.isEmpty())
				{
					m_Logger->alert("No Entry found for this version.");
				}
				else
				{
					base = new GeneralObject(Instance());
					base->SetLogoutTimout(obj.getField("logoutTimeout").numberLong());
					base->SetShutdownTimout(obj.getField("shutdownTimeout").numberLong());
				}
			}
			else
			{
				m_Logger->error("No DB connection.");
			}
			return base;
		}

	}
}