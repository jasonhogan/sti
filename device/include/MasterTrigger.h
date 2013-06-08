#ifndef STI_TIMINGENGINE_MASTERTRIGGER_H
#define STI_TIMINGENGINE_MASTERTRIGGER_H

#include "TimingEngineTypes.h"
#include "EventEngineState.h"


namespace STI
{
namespace TimingEngine
{

class MasterTrigger
{
public:

	virtual ~MasterTrigger() {}
	virtual bool waitForAll(EventEngineState state) = 0;	//true if the wait was successful; false if it aborted
	virtual void triggerAll() = 0;
	virtual void stopAll() = 0;

	//virtual bool waitForAll(const EngineID& engineID, EventEngineState state) = 0;	//true if the wait was successful; false if it aborted
	//virtual void triggerAll(const EngineInstance& engineInstance) = 0;
	//virtual void stopAll(const EngineID& engineID) = 0;

//	sequence<TTrigger> triggers;
//	TRepeatMode mode; // {Single, Continuous}

};




}
}

#endif
