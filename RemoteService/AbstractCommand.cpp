#include "AbstractCommand.h"
#include "RemoteBoxDevice.h"
#include "RemoteBoxWrapper.h"
#include "PowerStripeDevice.h"
#include "PowerSupplyDevice.h"
#include "FlashCmdProcessor.h"
#include "DeviceManager.h"

#include "WinApiHelper.h"

namespace RW{


	namespace CORE
	{
		AbstractCommand::AbstractCommand(QJsonObject* Obj, QObject *Parent) : Next(nullptr),
			m_logger(spdlog::get("file_logger"))
		{
			m_Parsing = false;
			//TODO das ist noch gefährlich ... ist aktuell nur da um dummys zu erzeugen
			if (Obj != nullptr)
			{
				m_CommandID = Obj->value("cmdid").toInt(-1);
				if (m_CommandID == -1)
					return;
				m_Src = Obj->value("src").toString("error");
				if (m_Src == "error")
					return;
				m_Dst = Obj->value("dst").toString("error");
				if (m_Dst == "error")
					return;
				m_ParameterList = Obj->value("param").toArray().toVariantList();

				const QMetaObject &mo = AbstractCommand::staticMetaObject;
				int index = mo.indexOfEnumerator("ExecutionVariantType");
				if (index != -1)
				{
					QMetaEnum metaEnum = mo.enumerator(index);
					quint8 var = metaEnum.keysToValue(Obj->value("executionvariant").toString().toUtf8());
					m_ExecutionVariant = (ExecutionVariantType)var;
					m_Parsing = true;
				}
			}
		}

		AbstractCommand::AbstractCommand(quint16 CmdId, QString Source, QString Destination, QString ExecutionVariant, QList<QVariant> Parameter, QObject *Parent) : Next(nullptr),
			m_logger(spdlog::get("file_logger")),
			m_CommandID(CmdId),
			m_Src(Source),
			m_Dst(Destination),
			m_Success(false),
			m_Result(0),
			m_ParameterList(Parameter)
		{
			QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
			quint8 var = enumVar.keysToValue(ExecutionVariant.toUtf8());
			m_ExecutionVariant = static_cast<AbstractCommand::ExecutionVariantType>(var);
		}

		void AbstractCommand::TransmitFinish(AbstractCommand* Cmd)
		{
			emit finished(Cmd);
		}

		void AbstractCommand::SetDevice(QObject* const Device)
		{}

		/****************************************************************************
		 _____      _              _____                                          _
		|  __ \    | |            / ____|                                        | |
		| |__) |___| | __ _ _   _| |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |
		|  _  // _ \ |/ _` | | | | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
		| | \ \  __/ | (_| | |_| | |___| (_) | | | | | | | | | | | (_| | | | | (_| |
		|_|  \_\___|_|\__,_|\__, |\_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
							 __/ |
							|___/
		*****************************************************************************/

		RelayCommand::RelayCommand(QJsonObject* Obj) : AbstractCommand(Obj, nullptr)
		{

		}

		void RelayCommand::SetDevice(QObject* const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
			m_RemoteBoxWrapper = (RemoteBoxWrapper::Wrapper*) manager->GetDevice(HW::DeviceManager::DeviceType::RemoteBox);
		}

		bool RelayCommand::Execute()
		{
			ushort val = 0;

			if (m_ParameterList.count() != 1)
			{
				m_logger->error("Wrong parameter amount in RelayCommand");
				this->m_Success = false;
			}

			if (m_ExecutionVariant == ExecutionVariantType::SET)
			{

				val = m_ParameterList.at(0).toUInt();
				this->m_Success = m_RemoteBoxWrapper->SetRelayState(val);
			}
			else if (m_ExecutionVariant == ExecutionVariantType::GET)
			{
				this->m_Success = m_RemoteBoxWrapper->GetRelayState(val);
				this->m_Result = val;
			}
			else
			{
				QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
				m_logger->alert("The execute function of RelayCommand don't have the execution variant: ") << enumVar.key((int)m_ExecutionVariant);
			}
			emit finished(this);
			return this->m_Success;
		}

		/****************************************************************************
		 _____ ____   _____                                          _
	    |_   _/ __ \ / ____|                                        | |
		  | || |  | | |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |
		  | || |  | | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
		 _| || |__| | |___| (_) | | | | | | | | | | | (_| | | | | (_| |
		|_____\____/ \_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|

		[0] = PinID, [1] = value
		*****************************************************************************/
		
		IOCommand::IOCommand(QJsonObject* Obj) : AbstractCommand(Obj, nullptr)
		{
		}
		  
		void IOCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
			m_RemoteBoxWrapper = (RemoteBoxWrapper::Wrapper*) manager->GetDevice(HW::DeviceManager::DeviceType::RemoteBox);
		}

		bool IOCommand::Execute()
		{
			bool val = false;
			
			if (m_ExecutionVariant == ExecutionVariantType::SET)
			{
				if (m_ParameterList.count() == 2)
				{
					quint16 id = m_ParameterList[0].toUInt();
					val = m_ParameterList[1].toUInt();
					this->m_Success = m_RemoteBoxWrapper->SetDIOPinState(id, val);
				}
				else
				{
					m_logger->error("Wrong parameter amount in IOCommand");
					this->m_Success = false;

				}
			}
			else if (m_ExecutionVariant == ExecutionVariantType::GET)
			{
				if (m_ParameterList.count() == 1)
				{
					quint16 id = m_ParameterList[0].toUInt();
					this->m_Success = m_RemoteBoxWrapper->GetDIOPinState(id, val);
					this->m_Result = val;
				}
				else
				{
					m_logger->error("Wrong parameter amount in IOCommand");
					this->m_Success = false;
				}
			}
			else
			{
				QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
				m_logger->alert("The execute function of IOCommand don't have the execution variant: ") << enumVar.key((int)m_ExecutionVariant);
			}
			emit finished(this);
			return this->m_Success;
		}


		/****************************************************************************
		 _    _  _____ ____   _____                                          _
		| |  | |/ ____|  _ \ / ____|                                        | |
		| |  | | (___ | |_) | |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |
		| |  | |\___ \|  _ <| |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
		| |__| |____) | |_) | |___| (_) | | | | | | | | | | | (_| | | | | (_| |
		 \____/|_____/|____/ \_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|

		[0] = HUB, [1] = state
		*****************************************************************************/

		USBCommand::USBCommand(QJsonObject* Obj) : AbstractCommand(Obj, nullptr)
		{
		}

		void USBCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
			m_RemoteBoxWrapper = (RemoteBoxWrapper::Wrapper*) manager->GetDevice(HW::DeviceManager::DeviceType::RemoteBox);
		}

		bool USBCommand::Execute()
		{
			quint8 state = 0;

			if (m_ExecutionVariant == ExecutionVariantType::SET)
			{
				if (m_ParameterList.count() == 2)
				{
					quint16 hub = m_ParameterList[0].toUInt();
					state = m_ParameterList[1].toUInt();
					this->m_Success = m_RemoteBoxWrapper->SetUsbState(hub, state);
				}
				else
				{
					m_logger->error("Wrong parameter amount in USBCommand");
					this->m_Success = false;
				}
			}
			else if (m_ExecutionVariant == ExecutionVariantType::GET)
			{
				if (m_ParameterList.count() == 1)
				{
					quint16 hub = m_ParameterList[0].toUInt();
					this->m_Success = m_RemoteBoxWrapper->GetUsbState(hub, state);
					this->m_Result = state;
				}
				else
				{
					m_logger->error("Wrong parameter amount in USBCommand");
					this->m_Success = false;
				}
			}
			else
			{
				QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
				m_logger->alert("The execute function of USBCommand don't have the execution variant: ") << enumVar.key((int)m_ExecutionVariant);
			}
			emit finished(this);
			return this->m_Success;
		}


		/****************************************************************************
		          _____   _____ _____                                          _
		    /\   |  __ \ / ____/ ____|                                        | |
		   /  \  | |  | | |   | |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |
		  / /\ \ | |  | | |   | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
		 / ____ \| |__| | |___| |___| (_) | | | | | | | | | | | (_| | | | | (_| |
		/_/    \_\_____/ \_____\_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|

		[0] = ADCId
		*****************************************************************************/

		ADCCommand::ADCCommand(QJsonObject* Obj) : AbstractCommand(Obj, nullptr)
		{
		}

		void ADCCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
			m_RemoteBoxWrapper = (RemoteBoxWrapper::Wrapper*) manager->GetDevice(HW::DeviceManager::DeviceType::RemoteBox);
		}

		bool ADCCommand::Execute()
		{
			short value = 0;

			if (m_ParameterList.count() != 1)
			{
				m_logger->error("Wrong parameter amount in ADCCommand");
				this->m_Success = false;
			}
			else
			{
				quint16 adcId = m_ParameterList[0].toUInt();

				if (m_ExecutionVariant == ExecutionVariantType::GET)
				{
					this->m_Success = m_RemoteBoxWrapper->GetAdcValue(adcId, value);
					this->m_Result = value;
				}
				else
				{
					QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
					m_logger->alert("The execute function of ADCCommand don't have the execution variant: ") << enumVar.key((int)m_ExecutionVariant);
				}
			}
			emit finished(this);
			return this->m_Success;
		}


		/****************************************************************************
		 _____          _____ _____                                          _
		|  __ \   /\   / ____/ ____|                                        | |
		| |  | | /  \ | |   | |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |
		| |  | |/ /\ \| |   | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
		| |__| / ____ \ |___| |___| (_) | | | | | | | | | | | (_| | | | | (_| |
		|_____/_/    \_\_____\_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|

		[0] = DACId, [1] = value
		*****************************************************************************/

		DACCommand::DACCommand(QJsonObject* Obj) : AbstractCommand(Obj, nullptr)
		{
		}

		void DACCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
			m_RemoteBoxWrapper = (RemoteBoxWrapper::Wrapper*) manager->GetDevice(HW::DeviceManager::DeviceType::RemoteBox);
		}

		bool DACCommand::Execute()
		{
			if (m_ParameterList.count() != 2)
			{
				m_logger->error("Wrong parameter amount in DACCommand");
				this->m_Success = false;
			}
			else
			{

				quint16 adcId = m_ParameterList[0].toUInt();
				short value = m_ParameterList[1].toUInt();

				if (m_ExecutionVariant == ExecutionVariantType::SET)
				{
					this->m_Success = m_RemoteBoxWrapper->SetDacValue(adcId, value);
				}
				else
				{
					QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
					m_logger->alert("The execute function of DACCommand don't have the execution variant: ") << enumVar.key((int)m_ExecutionVariant);
				}
			}
			emit finished(this);
			return this->m_Success;
		}

		/****************************************************************************
		 _____                       _____ _        _             _____                                          _
		|  __ \                     / ____| |      (_)           / ____|                                        | |
		| |__) |____      _____ _ _| (___ | |_ _ __ _ _ __   ___| |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |
		|  ___/ _ \ \ /\ / / _ \ '__\___ \| __| '__| | '_ \ / _ \ |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
		| |  | (_) \ V  V /  __/ |  ____) | |_| |  | | |_) |  __/ |___| (_) | | | | | | | | | | | (_| | | | | (_| |
		|_|   \___/ \_/\_/ \___|_| |_____/ \__|_|  |_| .__/ \___|\_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
												     | |
												     |_|
		[0] = Port, [1] = State
		*****************************************************************************/
		PowerStripeCommand::PowerStripeCommand(QJsonObject* Obj) : AbstractCommand(Obj,nullptr)
		{

		}

		void PowerStripeCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
			m_Device = (HW::PowerStripeDevice*) manager->GetDevice(HW::DeviceManager::DeviceType::PowerStripe);
		}


		bool PowerStripeCommand::Execute()
		{
			if (m_ParameterList.count() != 2)
			{
				m_logger->error("Wrong parameter amount in PowerStripeCommand");
				this->m_Success = false;
			}
			else
			{
				quint16 port = m_ParameterList[0].toUInt();
				
				if (m_ExecutionVariant == ExecutionVariantType::SET)
				{
					bool state = m_ParameterList[1].toUInt();
					HW::State hwState = (state == true) ? HW::State::ON : HW::State::OFF;
					this->m_Success = m_Device->SwitchPort(port, hwState);
				}
				else if (m_ExecutionVariant == ExecutionVariantType::GET)
				{
					HW::State hwState;
					this->m_Success = m_Device->GetPortState(port, hwState);
					this->m_Result = (hwState == HW::State::ON) ? true: false;
				}
				else
				{
					QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
					m_logger->alert("The execute function of PowerStripeCommand don't have the execution variant: ") << enumVar.key((int)m_ExecutionVariant);
				}
			}
			emit finished(this);
			return this->m_Success;
		}

		/****************************************************************************

		 _____                       _____                   _        _____                                          _
		|  __ \                     / ____|                 | |      / ____|                                        | |
		| |__) |____      _____ _ _| (___  _   _ _ __  _ __ | |_   _| |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |
		|  ___/ _ \ \ /\ / / _ \ '__\___ \| | | | '_ \| '_ \| | | | | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
		| |  | (_) \ V  V /  __/ |  ____) | |_| | |_) | |_) | | |_| | |___| (_) | | | | | | | | | | | (_| | | | | (_| |
		|_|   \___/ \_/\_/ \___|_| |_____/ \__,_| .__/| .__/|_|\__, |\_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
											  | |   | |       __/ |
											  |_|   |_|      |___/

		[0] = PinID, [1] = value
		*****************************************************************************/
		PowerSupplyCommand::PowerSupplyCommand(QJsonObject* Obj) : AbstractCommand(Obj, nullptr)
		{
		}

		void PowerSupplyCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
			m_Device = (HW::PowerSupplyDevice*) manager->GetDevice(HW::DeviceManager::DeviceType::PowerSupply);
		}

		bool PowerSupplyCommand::Execute()
		{

			if (m_ExecutionVariant == ExecutionVariantType::SET)
			{
				if (m_ParameterList.count() != 2)
				{
					m_logger->error("Wrong parameter amount in PowerSupplyCommand");
					emit finished(this);
					return this->m_Success = false;
				}

				double voltage = m_ParameterList[0].toDouble();
				double currentLimit = m_ParameterList[1].toDouble();

				if (!m_Device->SetVoltage(voltage))
				{
					m_logger->error("It wasn't possible to set the voltage.");
					this->m_Success = false;
					emit finished(this);
					return this->m_Success;
				}

				if (!m_Device->SetCurrentLimit(currentLimit))
				{
					m_logger->error("It wasn't possible to sete the current limit.");
					this->m_Success = false;
					emit finished(this);
					return this->m_Success;
				}

				/*all passed*/
				this->m_Success = true;
			}
			else if (m_ExecutionVariant == ExecutionVariantType::GET)
			{
				double voltage = 0, current = 0, currentLimit = 0;

				if (!m_Device->GetVoltage(voltage))
				{
					m_logger->error("It wasn't possible to read the voltage.");
					this->m_Success = false;
					emit finished(this);
					return this->m_Success;
				}

				if (!m_Device->GetCurrent(current))
				{
					m_logger->error("It wasn't possible to read the current.");
					this->m_Success = false;
					emit finished(this);
					return this->m_Success;
				}

				if (!m_Device->GetCurrentLimit(currentLimit))
				{
					m_logger->error("It wasn't possible to read the current limit.");
					this->m_Success = false;
					emit finished(this);
					return this->m_Success;
				}

				QList<QVariant> *ret = new QList<QVariant>();
				ret->append(voltage);
				ret->append(currentLimit);
				ret->append(current);

				this->m_Success = true;
				this->m_Result = *ret;
			}
			else
			{
				QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
				m_logger->alert("The execute function of PowerSupplyCommand don't have the execution variant: ") << enumVar.key((int)m_ExecutionVariant);
			}
			emit finished(this);
			return this->m_Success;
		}


		/****************************************************************************
		 _                  ____        _    _____                                          _
		| |                / __ \      | |  / ____|                                        | |
		| |     ___   __ _| |  | |_   _| |_| |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |
		| |    / _ \ / _` | |  | | | | | __| |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
		| |___| (_) | (_| | |__| | |_| | |_| |___| (_) | | | | | | | | | | | (_| | | | | (_| |
		|______\___/ \__, |\____/ \__,_|\__|\_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
					  __/ |
					  |___/

		[0] = PinID, [1] = value
		*****************************************************************************/

		LogOutCommand::LogOutCommand(QJsonObject* Obj) : AbstractCommand(Obj, nullptr)
		{
		}

		void LogOutCommand::SetDevice(QObject * const Device)
		{
		}

		bool LogOutCommand::Execute()
		{
			WinApiHelper helper;
			quint64 sessionId = 0;
			if(!helper.QueryActiveSession(sessionId))
			{
				SetResult("No active session on this remotehost");
				SetSuccess(false);
				emit finished(this);
				return false;
			}

			if (!helper.LogOff(sessionId))
			{
				SetResult("Can't logoff the user.");
				SetSuccess(false);
				emit finished(this);
				return false;
			}

			m_logger->error("LOGOUT");

			SetSuccess(true);
			emit finished(this);
			return true;
		}


		/****************************************************************************
		  _____ _           _      _                      _____                                          _
		 / ____| |         | |    | |                    / ____|                                        | |
		| (___ | |__  _   _| |_ __| | _____      ___ __ | |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |
		 \___ \| '_ \| | | | __/ _` |/ _ \ \ /\ / / '_ \| |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
		 ____) | | | | |_| | || (_| | (_) \ V  V /| | | | |___| (_) | | | | | | | | | | | (_| | | | | (_| |
		|_____/|_| |_|\__,_|\__\__,_|\___/ \_/\_/ |_| |_|\_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|

		*****************************************************************************/
		ShutdownCommand::ShutdownCommand(QJsonObject* Obj) : AbstractCommand(Obj, nullptr)
		{

		}

		void ShutdownCommand::SetDevice(QObject * const Device)
		{
		}

		bool ShutdownCommand::Execute()
		{
			WinApiHelper helper;

			if (m_Manager == nullptr)
			{
				SetResult("Can't shutdown the PC.");
				SetSuccess(false);
				emit finished(this);
				return false;
			}

			//All devices must be deinitilized
			if (!m_Manager->DeInit())
			{
				SetResult("Can't shutdown the PC.");
				SetSuccess(false);
				emit finished(this);
				return false;
			}

			//now the pc can be shutdown
			if (!helper.Shutdown())
			{
				SetResult("Can't shutdown the PC.");
				SetSuccess(false);
				emit finished(this);
				return false;
			}

			SetSuccess(true);
			emit finished(this);
			return true;
		}

		/****************************************************************************
		 ______ _           _      _____                                          _
		|  ____| |         | |    / ____|                                        | |
		| |__  | | __ _ ___| |__ | |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |
		|  __| | |/ _` / __| '_ \| |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
		| |    | | (_| \__ \ | | | |___| (_) | | | | | | | | | | | (_| | | | | (_| |
		|_|    |_|\__,_|___/_| |_|\_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|

		*****************************************************************************/
		FlashCommand::FlashCommand(QJsonObject* Obj) : AbstractCommand(Obj, nullptr)
		{

		}

		void FlashCommand::SetDevice(QObject * const Device)
		{
		}
		
		bool FlashCommand::Execute()
		{
			PLUGIN::FlashCmdProcessor *proc = new PLUGIN::FlashCmdProcessor();
			proc->moveToThread(&m_workerThread);

			connect(&m_workerThread, &QThread::finished, proc, &QObject::deleteLater);
			connect(proc, &PLUGIN::FlashCmdProcessor::finished, this, &AbstractCommand::TransmitFinish);
			connect(this, &FlashCommand::Start, proc, &PLUGIN::FlashCmdProcessor::DoWork);
			m_workerThread.start();
			emit Start(this);
			return true;
		}


	}
}
