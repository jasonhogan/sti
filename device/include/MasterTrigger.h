#ifndef STI_TIMINGENGINE_MASTERTRIGGER_H
#define STI_TIMINGENGINE_MASTERTRIGGER_H

#include "TimingEngineTypes.h"
#include "EventEngineState.h"


namespace STI
{
namespace TimingEngine
{

//TriggerDelegator<NetworkDevice> delegator(networkDeviceCollection);
//
//ServerEventEngine(delegator)
//{
//
////	delegator->setDevicesToTrigger(targetDevices);
//
//	triggerEngineManager->setupTrigger(targetDevices);	//has a generic TriggerDelegator, but derived class of EngineManager determines weather it uses local trigger resources or network reasoures.
//
//}
//
//class TriggerDelegator
//{
//	void 
//};
//
//
//class TriggerManager
//{
////	virtual void setServerEventEngineManager(const EventEngineManager_ptr& serverManager) = 0;
//	virtual void setDevicesToTrigger(const DeviceIDSet& deviceIDs) = 0;
//	virtual void waitForTrigger(const EngineInstance& instance) = 0;
//};

class MasterTrigger_Deprecated
{
public:

	//has Collector<ID, Derived>. gets the engine managers and sends the list to the triggerDevice

	virtual ~MasterTrigger() {}

//	virtual void waitForTrigger(const DeviceID& triggerDevice, EngineInstance& instance) =  0;
	//{
	//	LocalTrigger localTrigger(allDeviceEngineManagers);
	//	getDeviceEngineManager(triggerDevice)->trigger(instance, localTrigger);
	//}
//	virtual void setDevicesToTrigger(const DeviceIDSet& deviceIDs) = 0;
	//virtual bool waitForAll(EventEngineState state) = 0;	//true if the wait was successful; false if it aborted
	//virtual void triggerAll() = 0;
	//virtual void stopAll() = 0;

	//virtual bool waitForAll(const EngineID& engineID, EventEngineState state) = 0;	//true if the wait was successful; false if it aborted
	//virtual void triggerAll(const EngineInstance& engineInstance) = 0;
	//virtual void stopAll(const EngineID& engineID) = 0;

//	sequence<TTrigger> triggers;
//	TRepeatMode mode; // {Single, Continuous}

};

class Trigger
{
	virtual bool waitForAll(EventEngineState state) = 0;	//true if the wait was successful; false if it aborted
	virtual void triggerAll() = 0;
	virtual void stopAll() = 0;
};


}
}

#endif
