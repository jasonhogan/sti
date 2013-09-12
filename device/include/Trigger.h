#ifndef STI_TIMINGENGINE_TRIGGER_H
#define STI_TIMINGENGINE_TRIGGER_H

#include "TimingEngineTypes.h"

namespace STI
{
namespace TimingEngine
{

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
class Trigger
{
public:
	virtual ~Trigger() {}
	virtual bool waitForTrigger(const MasterTrigger_ptr& masterTrigger) = 0;
};



}
}

#endif

