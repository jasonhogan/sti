
#include "LocalTrigger.h"
#include "EngineID.h"
#include "EventEngineManager.h"

using STI::TimingEngine::LocalTrigger;
using STI::TimingEngine::EngineID;
using STI::TimingEngine::EngineInstance;
using STI::TimingEngine::EngineTimestamp;
using STI::TimingEngine::WeakEventEngineManagerVector_ptr;
using STI::TimingEngine::EventEngineState;
using STI::TimingEngine::EventEngineManager_ptr;


LocalTrigger::LocalTrigger(const STI::TimingEngine::EngineID& engineID, 
						   const EngineTimestamp& parseTimeStamp, 
						   const WeakEventEngineManagerVector_ptr& managers) : 
engineID_l(engineID), parseTimeStamp_l(parseTimeStamp), managers_l(managers)
{
}

bool LocalTrigger::waitForAll(EventEngineState state)
{
	//Waits for all engines to reach the same state. Aborts if none of the engines are in that state.
	//For example, if one the local engine is WaitingForTrigger, this loop will wait for all the
	//other engines to reach WaitingForTrigger as long as at least one (the local, for example)
	//is still in this state.
	bool keepWaiting = true;
	bool abort = false;
	unsigned numberInState = 0;
	EventEngineManager_ptr manager;

	 while(keepWaiting && !abort) {
		numberInState = 0;
		for(unsigned i = 0; i < managers_l->size(); i++) {
			if( (managers_l->at(i).expired()) || !(manager = managers_l->at(i).lock()) ) {
				abort = true;
				break;
			}

			if(manager != 0 && manager->inState(engineID_l, state)) {
				numberInState++;
			}
		}
		keepWaiting = (numberInState > 0) && (numberInState < managers_l->size());

		if(keepWaiting) {
			//timed sleep?
		}
	 }
	 return (numberInState == managers_l->size());
}


void LocalTrigger::triggerAll(const EngineTimestamp& playTimeStamp)
{
	STI::TimingEngine::EngineInstance engineInstance(engineID_l);
	engineInstance.parseTimestamp = parseTimeStamp_l;
	engineInstance.playTimestamp = playTimeStamp;

	EventEngineManager_ptr manager;
	for(unsigned i = 0; i < managers_l->size(); i++) {
		if( !(managers_l->at(i).expired()) && (manager = managers_l->at(i).lock()) ) {
			manager->trigger(engineInstance);
		}
	}
}


void LocalTrigger::stopAll()
{
	EventEngineManager_ptr manager;
	for(unsigned i = 0; i < managers_l->size(); i++) {
		if( !(managers_l->at(i).expired()) && (manager = managers_l->at(i).lock()) ) {
			manager->stop(engineID_l);
		}
	}
}

//
//
//
//
//LocalTrigger::LocalTrigger() : isMaster(false)
//{
//}
//
//void LocalTrigger::installMasterTrigger(const MasterTrigger_ptr& masterTrigger)
//{
//	masterTrigger_l = masterTrigger;
//	isMaster = true;
//}
//
//bool LocalTrigger::isMasterTrigger() const
//{
//	return isMaster;
//}
//
//void LocalTrigger::setIsMasterTrigger(bool master)
//{
//	isMaster = master;
//}
//
//bool LocalTrigger::waitForTrigger(const EngineID& engineID)
//{
//	if(masterTrigger_l) {
//		return masterTrigger_l->waitForAll(engineID, STI::TimingEngine::WaitingForTrigger);
//	}
//	return false;
//}
//
//void LocalTrigger::triggerAll(const EngineInstance& engineInstance)
//{
//	if(masterTrigger_l) {
//		masterTrigger_l->triggerAll(engineInstance);
//	}
//}
//
//void LocalTrigger::stopAll(const EngineID& engineID)
//{
//	if(masterTrigger_l) {
//		masterTrigger_l->stopAll(engineID);
//	}
//}
