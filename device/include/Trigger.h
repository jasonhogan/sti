#ifndef STI_TIMINGENGINE_TRIGGER_H
#define STI_TIMINGENGINE_TRIGGER_H

#include "TimingEngineTypes.h"

namespace STI
{
namespace TimingEngine
{

class Trigger
{
public:

	virtual void installMasterTrigger(MasterTrigger_ptr) = 0;

	virtual bool isMasterTrigger() = 0;
	virtual void setIsMasterTrigger(bool master) = 0;

	virtual void waitForTrigger() = 0;
	virtual void triggerAll() = 0;
	virtual void stopAll() = 0;

};

}
}

#endif

