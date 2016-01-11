#ifndef STI_TIMINGENGINE_TRIGGER_H
#define STI_TIMINGENGINE_TRIGGER_H

#include "TimingEngineTypes.h"
#include "EventEngineState.h"

namespace STI
{
namespace TimingEngine
{

class Trigger
{
public:

	virtual bool waitForAll(EventEngineState state) = 0;	//true if the wait was successful; false if it aborted
	virtual void triggerAll(const EngineTimestamp& playTimeStamp) = 0;
	virtual void stopAll() = 0;
};


//class Trigger
//{
//public:
//	virtual ~Trigger() {}
//
//	virtual void installMasterTrigger(const MasterTrigger_ptr& masterTrigger) = 0;
//
//	virtual bool isMasterTrigger() const = 0;
//	virtual void setIsMasterTrigger(bool master) = 0;
//
//	virtual bool waitForTrigger(const EngineID& engineID) = 0;
//	virtual void triggerAll(const EngineInstance& engineInstance) = 0;
//	virtual void stopAll(const EngineID& engineID) = 0;
//
//};
//
//class Trigger
//{
//public:
//	virtual ~Trigger() {}
//	virtual bool waitForTrigger(const MasterTrigger_ptr& masterTrigger) = 0;
//};


} //namespace TimingEngine
} //namespace STI

#endif

