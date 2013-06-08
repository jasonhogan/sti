#ifndef STI_TIMINGENGINE_LOCALMASTERTRIGGER_H
#define STI_TIMINGENGINE_LOCALMASTERTRIGGER_H

#include "MasterTrigger.h"
#include "TimingEngineTypes.h"

#include "EngineID.h"

namespace STI
{
namespace TimingEngine
{

class LocalMasterTrigger : public MasterTrigger
{
public:
	LocalMasterTrigger(const EventEngineManagerVector_ptr& managers, const EngineInstance& engineInstance);

	bool waitForAll(EventEngineState state);
	void triggerAll();
	void stopAll();
	
	//bool waitForAll(const EngineID& engineID, EventEngineState state);
	//void triggerAll(const EngineInstance& engineInstance);
	//void stopAll(const EngineID& engineID);

private:
	EventEngineManagerVector_ptr managers_l;
	EngineInstance engineInstance_l;
};


}
}

#endif
