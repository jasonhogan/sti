#ifndef STI_TIMINGENGINE_SERVEREVENTENGINE_H
#define STI_TIMINGENGINE_SERVEREVENTENGINE_H

#include "TimingEngineTypes.h"
#include "DeviceTypes.h"
#include "EventEngine.h"

#include "ParsingResultsTarget.h"

#include "DependencyTreeNode.h"

#include <boost/thread.hpp>
#include <sstream>

namespace STI
{
namespace TimingEngine
{

//all devices have one ServerEventEngine in the manager and several DeviceEventEngines
//server has several ServerEventEngines in its manager

//For Devices, put the ServerEventEngine in it's own manager or else it will conflict
//when it has to call play() on a DeviceEnventEngine controlling the device.
//ServerEventEngine never controls the hardware (no local load or play); it only controlls other
//managers.

//Or have each ServerEventEngine have a reference to another EventEngine that it calls after 
//"server" functionality that could be a DeviceEventEngine a the device.

////"server" function calls
//class LocalServerEventEngine : public EventEngine, ParsingResultsTarget
//{
//	//all {} functions
//}

////Maybe the collection can push events to Listeners and the ServerEventEngine could be a listener
//class DeviceCollection
//{
//public:
//	virtual void getDeviceIDs(set<DeviceID>& ids) = 0;
//	virtual void getDevice(const DeviceID& deviceID, Device_ptr& device);
//
//private:
//	DeviceMap_ptr devices;
////	void getMangers(set<EventEngineManagers>& ids);
//};



//class Device
//{
//	virtual void getDependentDeviceList(std::set<DeviceID>& devices) const = 0;
//	virtual const DeviceID& getDeviceID() const = 0;
//	virtual bool getEventEngineManager(const DeviceID& deviceID, const EventEngineManager_ptr& manager) = 0;
//};


class ServerEventEngine : public EventEngine, public ParsingResultsTarget
{
public:

	typedef STI::Utils::SynchronizedMap<STI::Device::DeviceID, TimingEventVector_ptr> DeviceToTimingEventsMap;
	typedef STI::Utils::DependencyTreeNode<STI::Device::DeviceID> DeviceDependencyTreeNode;
	typedef boost::shared_ptr<DeviceDependencyTreeNode> DeviceDependencyTreeNode_ptr;
	typedef STI::Utils::SynchronizedMap<STI::Device::DeviceID, DeviceDependencyTreeNode_ptr> DeviceDependencyMap;

	ServerEventEngine(const EngineID& engineID, const EventEngine_ptr& localEventEngine, const STI::Device::DeviceCollector_ptr& targetDevices);

	//virtual void parsePatch(const EngineInstance& oldInstance, const EngineInstance& newInstance, 
	//	const TimingEventVector& addedEvents, const TimingEventVector& subtractedEvents, 
	//	const ParsingResultsHandler_ptr& results) = 0;

///////////////////////


public:

	EventEngineState getState() const
	{
		return localEngine->getState();	
	}
	bool setState(EventEngineState newState)
	{
		return localEngine->setState(newState);
	}
	bool leaveState(EventEngineState state);

	bool inState(EventEngineState state) const;

	bool isSharedState(EventEngineState state) const
	{
		return state == Parsed || state == Loaded;
	}

	void preClear();
	void clear();
	void postClear();


	//void determineDeviceDependencies(DeviceDependencyMap& inputDependencies)
	//{
	//	DeviceDependencyMap deviceDependencies;
	//	std::set<DeviceID> dependents;

	//	for(all devices in inputDependencies) {
	//		dependents->clear();
	//		device->getDependentDeviceList(dependents);
	//		for(all dependents) {
	//			it = inputDependencies.find(*dependent);
	//			if(it != inputDependencies.end()) {
	//				//found in input list; increment dependency count for this device
	//				(*(it->second))++;
	//			}
	//			else {
	//				it = deviceDependencies.find(*dependent);
	//				if(it != deviceDependencies.end()) {
	//					//found in generated list
	//					(*(it->second))++;
	//				}
	//				else {
	//					determineDeviceDependencies(deviceDependencies...);

	//				}
	//			}

	//		}
	//	}
	//}

	//void addEventToDeviceEventList(const TimingEvent_ptr& evt, DeviceToTimingEventsMap& deviceEventsIn)
	//{
	//	TimingEventVector_ptr deviceEvents;
	//	DeviceID& deviceID = evt->channel()->deviceID();

	//	if(!deviceEventsIn.get(deviceID, deviceEvents)) {
	//		//This deviceID key is not in the map. Add it a new TimingEventVector.
	//		deviceEvents = TimingEventVector_ptr(new TimingEventVector());
	//		deviceEventsIn.add(deviceID, deviceEvents);
	//	}
	//	//Add this event to the associated device's event list
	//	deviceEvents->push_back(evt);
	//}
	void preParse();
	void parse(const EngineTimestamp& parseTimeStamp, 
		const TimingEventVector_ptr& eventsIn, const ParsingResultsHandler_ptr& results);

	void postParse();

	void preLoad();
	void load(const EngineTimestamp& parseTimeStamp);
	void postLoad();


	void preTrigger(double startTime, double endTime);
	void trigger(const MasterTrigger_ptr& delegatedTrigger);
	void trigger();
	void waitForTrigger();
	
	void prePlay(const EngineTimestamp& parseTimeStamp, const EngineTimestamp& playTimeStamp, 
		const PlayOptions_ptr& playOptions, const DocumentationOptions_ptr& docOptions);
	void play(const EngineTimestamp& parseTimeStamp, const EngineTimestamp& playTimeStamp, 
		const PlayOptions_ptr& playOptions, const DocumentationOptions_ptr& docOptions);

	void postPlay();


	//////////////////////////////

	void pause();

	void preResume() { localEngine->preResume(); }
	void resume();
	void resumeAt(double newTime);
	
	void stop();
	void postStop();

	void prePublishData() { localEngine->prePublishData(); }
	bool publishData(const EngineTimestamp& timestamp, TimingMeasurementGroup_ptr& data, const DocumentationOptions_ptr& documentation) { return false; }
	void postPublishData() { localEngine->postPublishData(); }


private:

	//Parse helper functions
	void divideEventList(const TimingEventVector_ptr& eventsIn, DeviceToTimingEventsMap& deviceEventsIn);
	DeviceDependencyTreeNode_ptr addDeviceNode(const STI::Device::DeviceID& deviceID, DeviceDependencyMap& deviceDependencies);
	void determineDeviceDependencies(DeviceDependencyMap& deviceDependencies);
	bool checkForCircularDependency(const STI::Device::DeviceIDSet& targetDevices, const DeviceDependencyMap& deviceDependencies);
	bool checkDeviceAvailability(const STI::Device::DeviceIDSet& targetDevices, std::stringstream& message) const;

	bool getDeviceEventEngineManager(const STI::Device::DeviceID& deviceID, EventEngineManager_ptr& manager) const;


	void handleParsingResults(const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		bool success, const std::string& errors, const TimingEventVector_ptr& eventsOut);

	ParsingResultsTarget_ptr serverParsingTarget;

	const EngineID serverEngineID;	//stored on initialization
	EventEngine_ptr localEngine;

//	DeviceList targetDevices;	//event partners; for the classic STI_Server, all device meet this criteria

	STI::Device::DeviceCollector_ptr registeredDeviceCollector;

	TimingEventGroupMap rawEvents;			//map<time, TimingEventGroup>;  Raw events grouped by time.
	DeviceToTimingEventsMap deviceEventsIn;
	TimingEventVector_ptr partnerEvents;
	DeviceDependencyMap deviceDependencies;
	STI::Device::DeviceIDSet parsingDevices;

	std::stringstream parsingErrors;
	bool parseSuccess;


	mutable boost::shared_mutex parseMutex;
	mutable boost::condition_variable_any parseCondition;

	//unsigned timingEventThreadPoolSize;
	//STI::Utils::QueuedEventHandler_ptr timingEventHandler; = new QueuedEventHandler(timingEventThreadPoolSize);
	//typedef std::map<DeviceID, QueuedEventEngineManager_ptr> DeviceToQueuedEngineManagerMap;
	//DeviceToQueuedEngineManagerMap deviceEngineManagers;


	EngineTimestamp lastParseTimeStamp;
	EngineTimestamp currentPlayTimeStamp;

	double parseTimeout_s;

//	const EngineID serverEngineID;	//this ServerEventEngine only commands other engines with this ID; resetable?
};


class ServerParsingResultsTargetWrapper : public ParsingResultsTarget
{
public:
	ServerParsingResultsTargetWrapper(ServerEventEngine* serverEngine) 
		: serverParsingTarget_l(serverEngine) {}

	void handleParsingResults(
		const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		bool success, const std::string& errors, 
		const STI::TimingEngine::TimingEventVector_ptr& eventsOut)
	{
		serverParsingTarget_l->handleParsingResults(deviceID, engineInstance, success, errors, eventsOut);
	}

private:
	ParsingResultsTarget* serverParsingTarget_l;
};


}
}

#endif

