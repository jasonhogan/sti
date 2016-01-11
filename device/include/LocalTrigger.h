#ifndef STI_TIMINGENGINE_LOCALTRIGGER_H
#define STI_TIMINGENGINE_LOCALTRIGGER_H

#include "Trigger.h"
#include "TimingEngineTypes.h"
#include "DeviceTypes.h"
#include "EngineID.h"

namespace STI
{
namespace TimingEngine
{

class LocalTrigger : public Trigger
{
public:
	LocalTrigger(const STI::TimingEngine::EngineID& engineID, 
		const EngineTimestamp& parseTimeStamp, 
		const STI::TimingEngine::WeakEventEngineManagerVector_ptr& engineManagers);

	virtual bool waitForAll(EventEngineState state);	//true if the wait was successful; false if it aborted
	virtual void triggerAll(const EngineTimestamp& playTimeStamp);
	virtual void stopAll();

private:

	EngineID engineID_l;
	EngineTimestamp parseTimeStamp_l;
	WeakEventEngineManagerVector_ptr managers_l;

};


//
//class LocalTrigger : public Trigger
//{
//public:
//	LocalTrigger(STI::Device::DeviceTimingEngineInterface& device) : device_l(device) {}
//	bool waitForTrigger(const MasterTrigger_ptr& masterTrigger)
//	{
//		//	masterTrigger->waitForAll(WaitingForTrigger);
//		device.waitForTrigger(masterTrigger);
//	}
//
//private:
//	STI::Device::DeviceTimingEngineInterface& device_l;
//};

//class LocalTrigger : public Trigger
//{
//public:
//
//	LocalTrigger();
//
//	void installMasterTrigger(const MasterTrigger_ptr& masterTrigger);
//
//	bool isMasterTrigger() const;
//	void setIsMasterTrigger(bool master);
//
//	virtual bool waitForTrigger(const EngineID& engineID);
//	virtual void triggerAll(const EngineInstance& engineInstance);
//	virtual void stopAll(const EngineID& engineID);
//
//private:
//	MasterTrigger_ptr masterTrigger_l;
//	bool isMaster;
//};

}
}

#endif

