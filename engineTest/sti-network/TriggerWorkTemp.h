



class NetworkDevice : public DeviceInterface
{
	bool getEventEngineManager(STI::TimingEngine::NetworkEventEngineManager_ptr& manager);
};

class LocalDevice : public DeviceInterface
{
	bool getEventEngineManager(STI::TimingEngine::LocalEventEngineManager_ptr& manager);
	virtual bool getEventEngineManager(STI::TimingEngine::EventEngineManager_ptr& manager)
	{
		LocalEventEngineManager_ptr localManager;
		return getEventEngineManager(localManager) && !((manager = localManager) == 0);
	}
};

class LocalTriggerManager	//LocalTrigger?
{
//	virtual void setServerEventEngineManager(const EventEngineManager_ptr& serverManager) = 0;
	void setDevicesToTrigger(const DeviceIDSet& deviceIDs)
	{
		LocalEventEngineManager_ptr manager;
		for(id : deviceIDs) {
			deviceCollector->getNode(id)->getEventEngineManager(manager);
			managers.push_back(manager);
		}
		managers.push_back(serverManager);
	}

	virtual void waitForTrigger(const EngineInstance& instance)
	{
		deviceManager->trigger(localTrigger);
	}

	LocalDeviceCollector_ptr deviceCollector;
	
	vector<LocalEventEngineManager_wptr> managers;
	LocalEventEngineManager_wptr deviceManager;
	LocalEventEngineManager_wptr serverManager;
};
class RemoteEventEngineManager : public EventEngineManager
{
	RemoteEventEngineManager(EventEngineManager_var manager);
};

class LocalTrigger : public Trigger
{
	void addManager();
	void clear();

	virtual bool waitForAll(EventEngineState state) = 0;	//true if the wait was successful; false if it aborted
	virtual void triggerAll(const EngineInstance& instance)
	{
		for(manager : managers) {
			manager->trigger(instance);
		}
	}
	virtual void stopAll(const EngineInstance& instance)
	{
		for(manager : managers) {
			manager->stop(instance.id);
		}
	}

	vector<EventEngineManager_ptr> managers;	//could be network or local. The Trigger object is for local devices.
};

networkEventEngineManger->trigger(instance, networkTrigger);

class NetworkEventEngineManager
{
	void trigger(EngineInstance instance, Trigger& trigger)
	{
		networkEngineManagerRef->trigger(convert(instance), convert(trigger).getNetworkTriggerRef());
	}
	void trigger(EngineInstance instance, NetworkTrigger& trigger)
	{
		networkEngineManagerRef->trigger(convert(instance), trigger.getTTrigger());
	}
};

interface NetworkEngineManager
{
	void trigger(in EngineInstance instance, in Trigger trigger);
};
struct TTrigger
{
	EventEngineManagerSeq manangers;
};

class NetworkTrigger : public Trigger	//trigger object that uses network reference
{
	virtual bool waitForAll(EventEngineState state) = 0;	//true if the wait was successful; false if it aborted
	virtual void triggerAll(const EngineInstance& instance)
	{
		//use the network reference to 
		managers = trigger.managers;
		for(manager : managers) {
			manager->trigger(instance);
		}
	}
	virtual void stopAll(const EngineInstance& instance)
	{
		managers = trigger.managers;
		for(manager : managers) {
			manager->stop(instance.id);
		}
	}

	TTrigger trigger;
};
