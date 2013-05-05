
#include "LocalTrigger.h"
#include "MasterTrigger.h"

using STI::TimingEngine::LocalTrigger;
using STI::TimingEngine::MasterTrigger_ptr;
using STI::TimingEngine::EngineInstance;
using STI::TimingEngine::EngineID;


LocalTrigger::LocalTrigger() : isMaster(false)
{
}

void LocalTrigger::installMasterTrigger(const MasterTrigger_ptr& masterTrigger)
{
	masterTrigger_l = masterTrigger;
	isMaster = true;
}

bool LocalTrigger::isMasterTrigger() const
{
	return isMaster;
}

void LocalTrigger::setIsMasterTrigger(bool master)
{
	isMaster = master;
}

bool LocalTrigger::waitForTrigger(const EngineID& engineID)
{
	if(masterTrigger_l) {
		return masterTrigger_l->waitForAll(engineID, STI::TimingEngine::WaitingForTrigger);
	}
	return false;
}

void LocalTrigger::triggerAll(const EngineInstance& engineInstance)
{
	if(masterTrigger_l) {
		masterTrigger_l->triggerAll(engineInstance);
	}
}

void LocalTrigger::stopAll(const EngineID& engineID)
{
	if(masterTrigger_l) {
		masterTrigger_l->stopAll(engineID);
	}
}
