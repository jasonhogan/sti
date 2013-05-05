#ifndef STI_TIMINGENGINE_LOCALTRIGGER_H
#define STI_TIMINGENGINE_LOCALTRIGGER_H

#include "Trigger.h"
#include "TimingEngineTypes.h"

namespace STI
{
namespace TimingEngine
{

class LocalTrigger : public Trigger
{
public:

	LocalTrigger();

	void installMasterTrigger(const MasterTrigger_ptr& masterTrigger);

	bool isMasterTrigger() const;
	void setIsMasterTrigger(bool master);

	virtual bool waitForTrigger(const EngineID& engineID);
	virtual void triggerAll(const EngineInstance& engineInstance);
	virtual void stopAll(const EngineID& engineID);

private:
	MasterTrigger_ptr masterTrigger_l;
	bool isMaster;
};

}
}

#endif

