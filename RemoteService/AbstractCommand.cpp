#include "AbstractCommand.h"
#include "RemoteBoxDevice.h"
#include "RemoteBoxWrapper.h"
#include "PowerStripeDevice.h"
#include "PowerSupplyDevice.h"
#include "FlashCmdProcessor.h"
#include "DeviceManager.h"
#include "RemoteDataConnectLibrary.h"
#include "WinApiHelper.h"

namespace RW{


	namespace CORE
	{
        AbstractCommand::AbstractCommand(const COM::Message Obj, QObject *Parent) : Next(nullptr),
			m_logger(spdlog::get("remoteservice"))
		{
			m_Parsing = false;

            m_CommandID = Obj.MessageID();
            if (m_CommandID == COM::MessageDescription::Amount)
                return;

            m_ParameterList = Obj.ParameterList();
            m_ExecutionVariant = Obj.ExcVariant();
            m_Result = Obj.Result();
            m_Success = Obj.Success();
            m_Parsing = true;

            Q_UNUSED(Parent);
		}

        AbstractCommand::AbstractCommand(COM::MessageDescription CmdId, QString Source, QString Destination, COM::Message::ExecutionVariant ExecutionVariant, QList<QVariant> Parameter, QObject *Parent) : Next(nullptr),
			m_logger(spdlog::get("remoteservice")),
			m_CommandID(CmdId),
			m_Src(Source),
			m_Dst(Destination),
			m_Success(false),
			m_Result(0),
			m_ParameterList(Parameter),
            m_ExecutionVariant(ExecutionVariant)
		{
		}

		void AbstractCommand::TransmitFinish(COM::Message Cmd)
		{
			emit finished(Cmd);
		}

		void AbstractCommand::SetDevice(QObject* const Device)
        {
            Q_UNUSED(Device)
        }

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

        RelayCommand::RelayCommand(const COM::Message Obj) : AbstractCommand(Obj, nullptr)
		{

		}

		void RelayCommand::SetDevice(QObject* const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
            m_RemoteBoxWrapper = (RemoteBoxWrapper::Wrapper*) manager->GetDevice(RW::PeripheralType::RemoteBox);
		}

		bool RelayCommand::Execute()
		{
			std::string ret = "";
			ushort val = 0;

			if (m_ParameterList.count() != 1)
			{
				m_logger->error("Wrong parameter amount in RelayCommand");
				this->m_Success = false;
			}

			if (m_ExecutionVariant == COM::Message::ExecutionVariant::SET)
			{
				val = m_ParameterList.at(0).toUInt();
				this->m_Success = m_RemoteBoxWrapper->SetRelayState(val, ret);
				if (!this->m_Success)
				{
					m_logger->error("SetRelayState failed with error: {}", ret);
				}
			}
            else if (m_ExecutionVariant == COM::Message::ExecutionVariant::GET)
			{
				this->m_Success = m_RemoteBoxWrapper->GetRelayState(val,ret);
				if (!this->m_Success)
				{
					m_logger->error("GetRelayState failed with error: {}", ret);
				}
				this->m_Result = val;
			}
			else
			{
				QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
				m_logger->warn("The execute function of RelayCommand don't have the execution variant: "); //<< enumVar.key((int)m_ExecutionVariant);
			}
			emit finished(this->toMessage());
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
		
		IOCommand::IOCommand(const COM::Message Obj) : AbstractCommand(Obj, nullptr)
		{
		}
		  
		void IOCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
            m_RemoteBoxWrapper = (RemoteBoxWrapper::Wrapper*) manager->GetDevice( PeripheralType::RemoteBox);
		}

		bool IOCommand::Execute()
		{
			bool val = false;
			std::string ret = "";
			
            if (m_ExecutionVariant == COM::Message::ExecutionVariant::SET)
			{
				if (m_ParameterList.count() == 2)
				{
					quint16 id = m_ParameterList[0].toUInt();
					val = m_ParameterList[1].toUInt();
					this->m_Success = m_RemoteBoxWrapper->SetDIOPinState(id, val,ret);
					if (!this->m_Success)
					{
						m_logger->error("SetDIOPinState failed with error: {}", ret);
					}
				}
				else
				{
					m_logger->error("Wrong parameter amount in IOCommand");
					this->m_Success = false;

				}
			}
            else if (m_ExecutionVariant == COM::Message::ExecutionVariant::GET)
			{
				if (m_ParameterList.count() == 1)
				{
					quint16 id = m_ParameterList[0].toUInt();
					this->m_Success = m_RemoteBoxWrapper->GetDIOPinState(id, val,ret);
					if (!this->m_Success)
					{
						m_logger->error("GetDIOPinState failed with error: {}", ret);
					}
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
				m_logger->warn("The execute function of IOCommand don't have the execution variant: "); //<< enumVar.key((int)m_ExecutionVariant);
			}
            emit finished(this->toMessage());
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

		USBCommand::USBCommand(const COM::Message Obj) : AbstractCommand(Obj, nullptr)
		{
		}

		void USBCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
            m_RemoteBoxWrapper = (RemoteBoxWrapper::Wrapper*) manager->GetDevice(PeripheralType::RemoteBox);
		}

		bool USBCommand::Execute()
		{
			std::string ret = "";
			quint8 state = 0;

            if (m_ExecutionVariant == COM::Message::ExecutionVariant::SET)
			{
				if (m_ParameterList.count() == 2)
				{
					quint16 hub = m_ParameterList[0].toUInt();
					state = m_ParameterList[1].toUInt();
					this->m_Success = m_RemoteBoxWrapper->SetUsbState(hub, state, ret);
					if (!this->m_Success)
					{
						m_logger->error("SetUsbState failed with error: {}", ret);
					}
				}
				else
				{
					m_logger->error("Wrong parameter amount in USBCommand");
					this->m_Success = false;
				}
			}
            else if (m_ExecutionVariant == COM::Message::ExecutionVariant::GET)
			{
				if (m_ParameterList.count() == 1)
				{
					quint16 hub = m_ParameterList[0].toUInt();
					this->m_Success = m_RemoteBoxWrapper->GetUsbState(hub, state, ret);
					if (!this->m_Success)
					{
						m_logger->error("GetUsbState failed with error: {}", ret);
					}
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
				m_logger->warn("The execute function of USBCommand don't have the execution variant: "); //<< enumVar.key((int)m_ExecutionVariant);
			}
            emit finished(this->toMessage());
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

		ADCCommand::ADCCommand(const COM::Message Obj) : AbstractCommand(Obj, nullptr)
		{
		}

		void ADCCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
			m_RemoteBoxWrapper = (RemoteBoxWrapper::Wrapper*) manager->GetDevice(PeripheralType::RemoteBox);
		}

		bool ADCCommand::Execute()
		{
			std::string ret = "";
			short value = 0;

			if (m_ParameterList.count() != 1)
			{
				m_logger->error("Wrong parameter amount in ADCCommand");
				this->m_Success = false;
			}
			else
			{
				quint16 adcId = m_ParameterList[0].toUInt();

                if (m_ExecutionVariant == COM::Message::ExecutionVariant::GET)
				{
					this->m_Success = m_RemoteBoxWrapper->GetAdcValue(adcId, value, ret);
					if (!this->m_Success)
					{
						m_logger->error("GetAdcValue failed with error: {}", ret);
					}
					this->m_Result = value;
				}
				else
				{
					QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
					m_logger->warn("The execute function of ADCCommand don't have the execution variant: "); //<< enumVar.key((int)m_ExecutionVariant);
				}
			}
            emit finished(this->toMessage());
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

		DACCommand::DACCommand(const COM::Message Obj) : AbstractCommand(Obj, nullptr)
		{
		}

		void DACCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
			m_RemoteBoxWrapper = (RemoteBoxWrapper::Wrapper*) manager->GetDevice(PeripheralType::RemoteBox);
		}

		bool DACCommand::Execute()
		{
			std::string ret = "";
			if (m_ParameterList.count() != 2)
			{
				m_logger->error("Wrong parameter amount in DACCommand");
				this->m_Success = false;
			}
			else
			{

				quint16 adcId = m_ParameterList[0].toUInt();
				short value = m_ParameterList[1].toUInt();

                if (m_ExecutionVariant == COM::Message::ExecutionVariant::SET)
				{
					this->m_Success = m_RemoteBoxWrapper->SetDacValue(adcId, value, ret);
					if (!this->m_Success)
					{
						m_logger->error("GetAdcValue failed with error: {}", ret);
					}
				}
				else
				{
					QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
					m_logger->warn("The execute function of DACCommand don't have the execution variant: "); //<< enumVar.key((int)m_ExecutionVariant);
				}
			}
            emit finished(this->toMessage());
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
		PowerStripeCommand::PowerStripeCommand(const COM::Message Obj) : AbstractCommand(Obj,nullptr)
		{

		}

		void PowerStripeCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
            m_Device = (HW::PowerStripeDevice*) manager->GetDevice(PeripheralType::PowerStripe);
		}


		bool PowerStripeCommand::Execute()
		{;
			if (m_ParameterList.count() != 2)
			{
				m_logger->error("Wrong parameter amount in PowerStripeCommand");
				this->m_Success = false;
			}
			else
			{
				quint16 port = m_ParameterList[0].toUInt();
				
                if (m_ExecutionVariant == COM::Message::ExecutionVariant::SET)
				{
					bool state = m_ParameterList[1].toUInt();
					HW::PortState hwState = (state == true) ? HW::PortState::ON : HW::PortState::OFF;
					this->m_Success = m_Device->SwitchPort(port, hwState);
				}
                else if (m_ExecutionVariant == COM::Message::ExecutionVariant::GET)
				{
					HW::PortState hwState;
					this->m_Success = m_Device->GetPortState(port, hwState);
					this->m_Result = (hwState == HW::PortState::ON) ? true : false;
				}
				else
				{
					QMetaEnum enumVar = this->staticMetaObject.enumerator(0);
					m_logger->warn("The execute function of PowerStripeCommand don't have the execution variant: "); //<< enumVar.key((int)m_ExecutionVariant);
				}
			}
            emit finished(this->toMessage());
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
		PowerSupplyCommand::PowerSupplyCommand(const COM::Message Obj) : AbstractCommand(Obj, nullptr)
		{
		}

		void PowerSupplyCommand::SetDevice(QObject * const Device)
		{
			HW::DeviceManager* manager = qobject_cast<HW::DeviceManager*>(Device);
			m_Device = (HW::PowerSupplyDevice*) manager->GetDevice(PeripheralType::PowerSupply);
		}

		bool PowerSupplyCommand::Execute()
		{

            if (m_ExecutionVariant == COM::Message::ExecutionVariant::SET)
			{
				if (m_ParameterList.count() != 2)
				{
					m_logger->error("Wrong parameter amount in PowerSupplyCommand");
                    emit finished(this->toMessage());
					return this->m_Success = false;
				}

				double voltage = m_ParameterList[0].toDouble();
				double currentLimit = m_ParameterList[1].toDouble();

				if (!m_Device->SetVoltage(voltage))
				{
					m_logger->error("It wasn't possible to set the voltage.");
					this->m_Success = false;
                    emit finished(this->toMessage());
					return this->m_Success;
				}

				if (!m_Device->SetCurrentLimit(currentLimit))
				{
					m_logger->error("It wasn't possible to sete the current limit.");
					this->m_Success = false;
                    emit finished(this->toMessage());
					return this->m_Success;
				}

				/*all passed*/
				this->m_Success = true;
			}
            else if (m_ExecutionVariant == COM::Message::ExecutionVariant::GET)
			{
				double voltage = 0, current = 0, currentLimit = 0;

				if (!m_Device->GetVoltage(voltage))
				{
					m_logger->error("It wasn't possible to read the voltage.");
					this->m_Success = false;
                    emit finished(this->toMessage());
					return this->m_Success;
				}

				if (!m_Device->GetCurrent(current))
				{
					m_logger->error("It wasn't possible to read the current.");
					this->m_Success = false;
                    emit finished(this->toMessage());
					return this->m_Success;
				}

				if (!m_Device->GetCurrentLimit(currentLimit))
				{
					m_logger->error("It wasn't possible to read the current limit.");
					this->m_Success = false;
                    emit finished(this->toMessage());
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
				m_logger->warn("The execute function of PowerSupplyCommand don't have the execution variant: "); //<< enumVar.key((int)m_ExecutionVariant);
			}
            emit finished(this->toMessage());
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

		LogOutCommand::LogOutCommand(const COM::Message Obj) : AbstractCommand(Obj, nullptr)
		{
		}

		void LogOutCommand::SetDevice(QObject * const Device)
		{
            Q_UNUSED(Device)
		}

		bool LogOutCommand::Execute()
		{
			//WinApiHelper helper;
			//quint64 sessionId = 0;
			//if(!helper.QueryActiveSession(sessionId))
			//{
			//	SetResult("No active session on this remotehost");
			//	SetSuccess(false);
			//	emit finished(this);
			//	return false;
			//}

			//if (!helper.LogOff(sessionId))
			//{
			//	SetResult("Can't logoff the user.");
			//	SetSuccess(false);
			//	emit finished(this);
			//	return false;
			//}

			m_logger->error("LOGOUT");

			SetSuccess(true);
            emit finished(this->toMessage());
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
		ShutdownCommand::ShutdownCommand(const COM::Message Obj) : AbstractCommand(Obj, nullptr)
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
                emit finished(this->toMessage());
				return false;
			}

			//All devices must be deinitilized
			if (!m_Manager->DeInit())
			{
				SetResult("Can't shutdown the PC.");
				SetSuccess(false);
                emit finished(this->toMessage());
				return false;
			}

			//now the pc can be shutdown
			//if (!helper.Shutdown())
			//{
			//	SetResult("Can't shutdown the PC.");
			//	SetSuccess(false);
			//	emit finished(this);
			//	return false;
			//}

			SetSuccess(true);
            emit finished(this->toMessage());
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
		FlashCommand::FlashCommand(const COM::Message Obj) : AbstractCommand(Obj, nullptr)
		{

		}

		void FlashCommand::SetDevice(QObject * const Device)
		{
		}
		
		bool FlashCommand::Execute()
		{
			PLUGIN::FlashCmdProcessor *proc = new PLUGIN::FlashCmdProcessor();
			proc->moveToThread(&m_workerThread);

			//connect(&m_workerThread, &QThread::finished, proc, &QObject::deleteLater);
			//connect(proc, &PLUGIN::FlashCmdProcessor::finished, this, &AbstractCommand::TransmitFinish);
			//connect(this, &FlashCommand::Start, proc, &PLUGIN::FlashCmdProcessor::DoWork);
			m_workerThread.start();
			emit Start(this);
			return true;
		}
	}
}
