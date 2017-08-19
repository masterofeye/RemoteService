#include <assert.h>
#include "StateMachine.h"
#include <spdlog\spdlog.h>

namespace RW{
	namespace CORE{
		StateMachine::StateMachine(unsigned char maxStates) :
			_maxStates(maxStates),
			m_CurrentState(0),
			_eventGenerated(false),
			_pEventData(NULL)
		{
		}

		// generates an external event. called once per external event 
		// to start the state machine executing
		void StateMachine::ExternalEvent(unsigned char newState,
			EventData* pData)
		{
			// if we are supposed to ignore this event
			if (newState == EVENT_IGNORED) {
				// just delete the event data, if any
				if (pData)
					delete pData;
			}
			else {
				// TODO - capture software lock here for thread-safety if necessary

				// generate the event and execute the state engine
				InternalEvent(newState, pData);
				

				// TODO - release software lock here 
			}
		}

		// generates an internal event. called from within a state 
		// function to transition to a new state
		void StateMachine::InternalEvent(unsigned char newState,
			EventData* pData)
		{
			if (pData == NULL)
				pData = new EventData();

			_pEventData = pData;
			_eventGenerated = true;
			m_CurrentState = newState;
			StateEngine();
		}

		// the state engine executes the state machine states
		void StateMachine::StateEngine(void)
		{
			EventData* pDataTemp = NULL;

			// while events are being generated keep executing states
			while (_eventGenerated) {
				pDataTemp = _pEventData;  // copy of event data pointer
				_pEventData = NULL;       // event data used up, reset ptr
				_eventGenerated = false;  // event used up, reset flag

				if (m_CurrentState > _maxStates)
				{
					std::shared_ptr<spdlog::logger> Logger = spdlog::get("remoteservice");
					Logger->critical("Statemachine breaks");
					if (pDataTemp) {
						delete pDataTemp;
						pDataTemp = NULL;
					}
					return;
				}


				// get state map
				const StateStruct* pStateMap = GetStateMap();

				// execute the state passing in event data, if any
				(this->*pStateMap[m_CurrentState].pStateFunc)(pDataTemp);

				// if event data was used, then delete it
				if (pDataTemp) {
					delete pDataTemp;
					pDataTemp = NULL;
				}
			}
		}
	}
}