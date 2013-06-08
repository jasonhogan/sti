
#include "ServerEventEngine.h"
#include "EventEngineManager.h"
#include "DeviceInterface.h"
#include "TimingEvent.h"
#include "Collector.h"
#include "EngineID.h"
#include "ParsingResultsHandler.h"
#include "Channel.h"

#include <sstream>

using STI::TimingEngine::ServerEventEngine;
using STI::TimingEngine::EngineID;
using STI::TimingEngine::EventEngine_ptr;
using STI::Device::DeviceCollector_ptr;
using STI::TimingEngine::EventEngineState;

using STI::Device::DeviceID;
using STI::TimingEngine::EventEngineManager_ptr;
using STI::TimingEngine::TimingEventVector_ptr;
using STI::Device::DeviceIDSet;

using STI::TimingEngine::EngineTimestamp;
using STI::TimingEngine::PlayOptions_ptr;
using STI::TimingEngine::DocumentationOptions_ptr;
using STI::TimingEngine::MasterTrigger_ptr;
using STI::TimingEngine::ParsingResultsHandler_ptr;


ServerEventEngine::ServerEventEngine(const EngineID& engineID, const EventEngine_ptr& localEventEngine, const DeviceCollector_ptr& targetDevices) 
: serverEngineID(engineID), localEngine(localEventEngine), registeredDeviceCollector(targetDevices)
{
	parseTimeout_s = 0.5;
	serverParsingTarget = ParsingResultsTarget_ptr(new STI::TimingEngine::ServerParsingResultsTargetWrapper(this));
}

bool ServerEventEngine::inState(EventEngineState state) const
{
	using STI::Device::DeviceIDSet;
	if(isSharedState(state)) {
		bool result = localEngine->inState(state);

		DeviceIDSet deviceIDs;
		registeredDeviceCollector->getIDs(deviceIDs);
		EventEngineManager_ptr manager;
		for(DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {

			if(getDeviceEventEngineManager(*id, manager)) {
				result &= manager->inState(serverEngineID, state);
			}
		}
		return result;
	}
	else {
		return localEngine->inState(state);	
	}
}

bool ServerEventEngine::leaveState(EventEngineState state)
{
	using STI::Device::DeviceIDSet;
	
	bool result = localEngine->leaveState(state);

	//DeviceIDSet deviceIDs;
	//registeredDeviceCollector->getIDs(deviceIDs);
	//EventEngineManager_ptr manager;
	//for(DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {

	//	if(getDeviceEventEngineManager(*id, manager)) {
	//		result &= manager->leaveState(serverEngineID, state);
	//	}
	//}
	return result;

}

void ServerEventEngine::preClear()
{
	localEngine->preClear();
}

void ServerEventEngine::clear()
{
	localEngine->clear();

	//DeviceToQueuedEngineManagerMap::iterator manager;
	//for(manager = deviceEngineManagers.begin(); manager != deviceEngineManagers.end(); manager++) {
	//	manager->second->clear(serverEngineID);
	//}

	STI::Device::DeviceIDSet deviceIDs;
	registeredDeviceCollector->getIDs(deviceIDs);
	EventEngineManager_ptr manager;
	for(STI::Device::DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
			manager->clear(serverEngineID);
		}
	}
}

bool ServerEventEngine::getDeviceEventEngineManager(const DeviceID& deviceID, EventEngineManager_ptr& manager) const
{
	STI::Device::DeviceInterface_ptr device;
	if(registeredDeviceCollector->get(deviceID, device) && device != 0 
		&& device->getEventEngineManager(manager) && manager != 0) {
			return true;
	}
	return false;
}

void ServerEventEngine::postClear()
{
	localEngine->postClear();
}

void ServerEventEngine::divideEventList(const TimingEventVector_ptr& eventsIn, DeviceToTimingEventsMap& deviceEventsIn)
{
	//Breaks up the full event list into lists of events for each device.
	TimingEventVector_ptr deviceEvents;

	//for(unsigned i = 0; i < eventsIn->size(); i++) {
	//	addEventToDeviceEventList(eventsIn->at(i), deviceEventsIn);
	//}

	for(unsigned i = 0; i < eventsIn->size(); i++) {
		STI::Device::DeviceID deviceID = eventsIn->at(i)->channel().deviceID();

		if(!deviceEventsIn.get(deviceID, deviceEvents)) {
			//This deviceID key is not in the map. Add it a new TimingEventVector.
			deviceEvents = TimingEventVector_ptr(new TimingEventVector());
			deviceEventsIn.add(deviceID, deviceEvents);
		}
		//Add this event to the associated device's event list
		deviceEvents->push_back( eventsIn->at(i) );
	}
}

ServerEventEngine::DeviceDependencyTreeNode_ptr ServerEventEngine::addDeviceNode(const DeviceID& deviceID, ServerEventEngine::DeviceDependencyMap& deviceDependencies)
{
	//Recursively add devices and their targets to the dependency map.

	//using STI::TimingEngine::ServerEventEngine::DeviceDependencyTreeNode;
	//using STI::TimingEngine::ServerEventEngine::DeviceDependencyTreeNode_ptr;

	ServerEventEngine::DeviceDependencyTreeNode_ptr newNode;
	STI::Device::DeviceIDSet deviceIDList;
	STI::Device::DeviceInterface_ptr device;
	STI::Device::DeviceCollector_ptr partnerCollector;

	if(!deviceDependencies.contains(deviceID)) {
		//This is a new deviceID. Generate a new node and add it.
		newNode = ServerEventEngine::DeviceDependencyTreeNode_ptr(new ServerEventEngine::DeviceDependencyTreeNode(deviceID));
		deviceDependencies.add(deviceID, newNode);

		//Determine this node's device targets. Attempt to add them to the dependency list.
		if( registeredDeviceCollector->get(deviceID, device) && device != 0) {
			if(device->getPartnerCollector(partnerCollector) && partnerCollector != 0) {
				partnerCollector->getIDs(deviceIDList);

				for(DeviceIDSet::iterator id = deviceIDList.begin(); id != deviceIDList.end(); id++) {
					//Attempt to add the target device to the dependecy list.
					//If the deviceID is already listed, the call to addDeviceNode will return the existing node.
					newNode->addTargetNode(
						addDeviceNode(*id, deviceDependencies));
				}
			}
		}
	} else {
		//Dependency list already include this deviceID. Return the node.
		deviceDependencies.get(deviceID, newNode);
	}
	return newNode;
}

void ServerEventEngine::determineDeviceDependencies(ServerEventEngine::DeviceDependencyMap& deviceDependencies)
{
	DeviceIDSet ids;
	deviceEventsIn.getKeys(ids);

	for(DeviceIDSet::iterator id = ids.begin(); id != ids.end(); id++) {
		addDeviceNode(*id, deviceDependencies);
	}
}

void ServerEventEngine::preParse()
{
	localEngine->preParse();
}

bool ServerEventEngine::checkForCircularDependency(const std::set<DeviceID>& targetDevices, const ServerEventEngine::DeviceDependencyMap& deviceDependencies)
{
	bool foundLoop = true;
	DeviceDependencyTreeNode_ptr deviceDependencyNode;
	for(DeviceIDSet::const_iterator target = targetDevices.begin(); target != targetDevices.end(); target++) {
		if(deviceDependencies.get(*target, deviceDependencyNode) && deviceDependencyNode != 0) {
			//If every device in targetDevices has non-zero dependency count, then there must be a loop.
			//(i.e., there is no device that can proceed without waiting for another device to parse first.)
			foundLoop &= (deviceDependencyNode->dependencyCount() > 0);
		}
	}
	return foundLoop;
}

bool ServerEventEngine::checkDeviceAvailability(const DeviceIDSet& targetDevices, std::stringstream& message) const
{
	bool allFound = true;
	
	for(DeviceIDSet::const_iterator deviceID = targetDevices.begin(); deviceID != targetDevices.end(); deviceID++) {
		allFound &= registeredDeviceCollector->contains(*deviceID);
		message << "Missing device: dev(" 
			<< deviceID->getName() << ", "
			<< deviceID->getAddress() << ", "
			<< deviceID->getModule() << ")" << std::endl;
	}
	return allFound;
}

void ServerEventEngine::parse(const EngineTimestamp& parseTimeStamp, 
	const TimingEventVector_ptr& eventsIn, const ParsingResultsHandler_ptr& results)
{
	lastParseTimeStamp = parseTimeStamp;

	STI::TimingEngine::EngineInstance engineInstance(serverEngineID);
	engineInstance.parseTimestamp = lastParseTimeStamp;


//	DeviceToTimingEventsMap deviceEventsIn;
	DeviceIDSet targetDevices;
	
	divideEventList(eventsIn, deviceEventsIn);	//eventsInByDevice

	determineDeviceDependencies(deviceDependencies);
	
	//Get all the devices that MAY have events.
	deviceDependencies.getKeys(targetDevices);	
	
	std::stringstream errMessage;

	//Check device availability
	if(!checkDeviceAvailability(targetDevices, errMessage)) {
		//Missing devices
	}

	//Parse events on all devices
	TimingEventVector_ptr deviceEvents;
	DeviceDependencyTreeNode_ptr deviceDependencyNode;
	unsigned currentParsingDeviceCount;
	std::set<DeviceID>::iterator targetID;

	using STI::TimingEngine::ParsingResultsHandler;
	STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler;

	partnerEvents = TimingEventVector_ptr(new TimingEventVector());

	boost::system_time wakeTime;
	
	parseSuccess = true;
	parsingErrors.str("");

	while(inState(Parsing) && targetDevices.size() > 0) 
	{
		//Lock parse results handler for an entire pass at the target devices.
		boost::unique_lock< boost::shared_mutex > parseLock(parseMutex);
		
		EventEngineManager_ptr manager;
		TimingEventVector_ptr events;
		//Loop through all devices and attempt to parse if they have no depenendecies.
		for(targetID = targetDevices.begin(); targetID != targetDevices.end(); targetID++) {
			if(deviceDependencies.get(*targetID, deviceDependencyNode) && deviceDependencyNode != 0) {
				if(deviceDependencyNode->dependencyCount() == 0) {
					//Any device dependencies are gone, so this device is ready to parse.
					if(deviceEventsIn.get(*targetID, deviceEvents) && deviceEvents != 0 && deviceEvents->size() > 0) {
						//?parseDevice(*targetID, parsingDevices)		parseDevice(DeviceID& deviceID, DeviceIDSet& parsingQueue)
						//Verified that this device has events to parse. Now parse them.
						parsingDevices.insert(*targetID);
						if(getDeviceEventEngineManager(*targetID, manager) 
							&& deviceEventsIn.get(*targetID, events) && events != 0)
						{
							parsingHandler = ParsingResultsHandler_ptr(
								new ParsingResultsHandler(*targetID, engineInstance, serverParsingTarget));
					
							manager->parse(engineInstance, events, parsingHandler);
						}
					}
				}
			}
		}

		//Erase all the parsing devices from the list of target devices.
		for(std::set<DeviceID>::iterator it = parsingDevices.begin(); it != parsingDevices.end(); it++) {
			targetID = targetDevices.find(*it);
			if(targetID != targetDevices.end()) {
				targetDevices.erase(targetID);
			}
		}

		//Save the current number of parsing devices. If this changes it means that a device finished parsing
		//and so we should stop waiting and try to parse more devices using the updated dependencies.
		currentParsingDeviceCount = parsingDevices.size();

		//Check for circular dependency
		if(currentParsingDeviceCount == 0 && targetDevices.size() > 0) {
			//No devices were ready to parse on the last pass. Possible circular dependency.
			if(checkForCircularDependency(targetDevices, deviceDependencies)) {
				//Circular dependency!
			}
		}

	

		//Sleep while waiting for devices to parse
		while( inState(Parsing) && parsingDevices.size() > 0 
			&& currentParsingDeviceCount == parsingDevices.size())
		{
			wakeTime = boost::get_system_time() 
				+ boost::posix_time::milliseconds( static_cast<long>(parseTimeout_s * 1000) );
			parseCondition.timed_wait(parseMutex, wakeTime);
		}
	}


	//Parse local engine
//	localEngine->parse(parseTimeStamp, deviceEventsIn[localDeviceID], serverResultsHandler);


	results->returnResults(parseSuccess, parsingErrors.str(), partnerEvents);

}



	//	set<DeviceID> independentDevices;

	//ParsingResultsHandler_ptr& results


	////parse all independent devices
	//for(device = independentDevices.begin(); device != independentDevices.end(); device++) {
	//	
	//	deviceEngineManagers[*device]->parse(engineInstance, deviceEventsIn[*device], serverResultsHandler);
	//}

	//while(inState(Parsing) && waitingForDevices)
	//{
	//	while no devices are ready && server is inState(Parsing)
	//		sleep;
	//}


	//while(devicesToParse)
	//{
	//	if(isIndependent(device) {
	//		deviceEngineManagers[*device]->parse(engineInstance, deviceEventsIn[*device, serverResultsHandler);
	//	}
	//	while(no more devices are ready)
	//		sleep
	//}


void ServerEventEngine::handleParsingResults(
	const STI::Device::DeviceID& deviceID, 
	const STI::TimingEngine::EngineInstance& engineInstance,
	bool success, const std::string& errors, 
	const TimingEventVector_ptr& eventsOut)
{
	boost::unique_lock< boost::shared_mutex > parseLock(parseMutex);

	if(engineInstance.parseTimestamp != lastParseTimeStamp)
		return;

	parseSuccess &= success;
	parsingErrors << errors;

	if(success) {

		//Append partner events
		partnerEvents->insert(partnerEvents->end(), eventsOut->begin(), eventsOut->end());

		divideEventList(eventsOut, deviceEventsIn);

		DeviceIDSet::iterator device = parsingDevices.find(deviceID);

		DeviceDependencyTreeNode_ptr deviceDependencyNode;
		if(device != parsingDevices.end()) {
			//Clear this devices dependencies so target devices can parse.
			if(deviceDependencies.get(*device, deviceDependencyNode) && deviceDependencyNode != 0) {
				deviceDependencyNode->releaseTargets();
			}

			//Remove device from parsing devices
			parsingDevices.erase(device);
		}
		parseCondition.notify_one();
	}
}


void ServerEventEngine::postParse()
{
	localEngine->postParse();
}

void ServerEventEngine::preLoad()
{
	localEngine->preLoad();
}

void ServerEventEngine::load(const EngineTimestamp& parseTimeStamp)
{
//	prepareForDevicesToLoad();

	STI::TimingEngine::EngineInstance engineInstance(serverEngineID);
	engineInstance.parseTimestamp = parseTimeStamp;

	localEngine->load(parseTimeStamp);

	DeviceIDSet deviceIDs;
	deviceEventsIn.getKeys(deviceIDs);	//devices with events

	EventEngineManager_ptr manager;
	for(DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
			manager->load(engineInstance);
		}
	}

//	waitForDevicesToLoad();
}

void ServerEventEngine::postLoad()
{
	localEngine->postLoad();
}

void ServerEventEngine::prePlay(const EngineTimestamp& parseTimeStamp, const EngineTimestamp& playTimeStamp, 
	const PlayOptions_ptr& playOptions, 
	const DocumentationOptions_ptr& docOptions) 
{
	localEngine->prePlay(parseTimeStamp, playTimeStamp, playOptions, docOptions);

	STI::TimingEngine::EngineInstance engineInstance(serverEngineID);
	engineInstance.parseTimestamp = parseTimeStamp;
	engineInstance.playTimestamp = playTimeStamp;

	DeviceIDSet deviceIDs;
	deviceEventsIn.getKeys(deviceIDs);	//devices with events

	EventEngineManager_ptr manager;
	for(STI::Device::DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
			manager->play(engineInstance, playOptions, docOptions);
		}
	}
}

void ServerEventEngine::preTrigger(double startTime, double endTime)
{
	localEngine->preTrigger(startTime, endTime);
}

void ServerEventEngine::trigger(const MasterTrigger_ptr& delegatedTrigger)
{
	localEngine->trigger(delegatedTrigger);
}

void ServerEventEngine::trigger()
{
	localEngine->trigger();
}

void ServerEventEngine::waitForTrigger()
{
	localEngine->waitForTrigger();
}

void ServerEventEngine::play(const EngineTimestamp& parseTimeStamp, const EngineTimestamp& playTimeStamp, 
	const PlayOptions_ptr& playOptions, const DocumentationOptions_ptr& docOptions)
{
	localEngine->play(parseTimeStamp, playTimeStamp, playOptions, docOptions);
}

void ServerEventEngine::postPlay()
{
	localEngine->postPlay();
}


void ServerEventEngine::pause()
{
	localEngine->pause();

	STI::Device::DeviceIDSet deviceIDs;
	registeredDeviceCollector->getIDs(deviceIDs);
	EventEngineManager_ptr manager;
	for(STI::Device::DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
			manager->pause(serverEngineID);
		}
	}
}

void ServerEventEngine::resume()
{
	localEngine->resume();
	
	STI::TimingEngine::EngineInstance engineInstance(serverEngineID);
	engineInstance.parseTimestamp = lastParseTimeStamp;

	STI::Device::DeviceIDSet deviceIDs;
	registeredDeviceCollector->getIDs(deviceIDs);
	EventEngineManager_ptr manager;
	for(STI::Device::DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
			manager->resume(engineInstance);
		}
	}
}

void ServerEventEngine::resumeAt(double newTime)
{
	localEngine->resumeAt(newTime);

	STI::Device::DeviceIDSet deviceIDs;
	registeredDeviceCollector->getIDs(deviceIDs);
	EventEngineManager_ptr manager;
	for(STI::Device::DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
//			manager->resume(serverEngineID);
		}
	}
}




void ServerEventEngine::stop()
{
	localEngine->stop();

	STI::Device::DeviceIDSet deviceIDs;
	registeredDeviceCollector->getIDs(deviceIDs);
	EventEngineManager_ptr manager;
	for(STI::Device::DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
			manager->stop(serverEngineID);
		}
	}
}

void ServerEventEngine::postStop()
{
	localEngine->postStop();
}