
#include "ServerEventEngine.h"
#include "EventEngineManager.h"
#include "DeviceInterface.h"
#include "TimingEvent.h"
#include "Collector.h"
#include "EngineID.h"
#include "ParsingResultsHandler.h"
#include "Channel.h"
#include "TextPosition.h"
#include "LocalMasterTrigger.h"
#include "EngineCallbackHandler.h"


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
using STI::TimingEngine::EngineCallbackHandler_ptr;
using STI::TimingEngine::MeasurementResultsHandler_ptr;


ServerEventEngine::ServerEventEngine(const EngineID& engineID, const EventEngine_ptr& localEventEngine, 
									 const DeviceCollector_ptr& targetDevices, const STI::Device::DeviceID& serverID) : 
serverID(serverID), stiTriggerDeviceID("STI_Trigger", serverID.getAddress(), serverID.getModule()), 
serverEngineID(engineID), localEngine(localEventEngine), registeredDeviceCollector(targetDevices)
//: serverEngineID(engineID), localEngine(localEventEngine), registeredDeviceCollector(targetDevices), 
//serverID(serverID), localEngineManager(localEventEngineManager), stiTriggerDeviceID("STI_Trigger", serverID.getAddress(), serverID.getModule())
{

//localEngineManager(localEventEngineManager),



	parseTimeout_s = 0.5;
	serverParsingTarget = ParsingResultsTarget_ptr(new STI::TimingEngine::ServerParsingResultsTargetWrapper(this));
	engineCallbackTarget = EngineCallbackTarget_ptr(new STI::TimingEngine::ServerEngineCallbackTargetWrapper(this));

	targetManagers = EventEngineManagerVector_ptr(new EventEngineManagerVector());
	partnerEvents = TimingEventVector_ptr(new TimingEventVector());






	//Add EventEngine to other engines
//	DeviceIDSet deviceIDs;
//	registeredDeviceCollector->getIDs(deviceIDs);
//	STI::Device::DeviceInterface_ptr device;
//	EventEngineManager_ptr manager;
//	for(DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {
//		if(registeredDeviceCollector->get(*id, device) && device != 0) {
//			device->addEventEngine(engineID);
//		}
//
////		if(getDeviceEventEngineManager(*id, manager)) {
////			manager->addEventEngine(serverEngineID);
////		}
//	}
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

void ServerEventEngine::clear(const EngineCallbackHandler_ptr& callBack)
{
	localEngine->clear(callBack);

	//DeviceToQueuedEngineManagerMap::iterator manager;
	//for(manager = deviceEngineManagers.begin(); manager != deviceEngineManagers.end(); manager++) {
	//	manager->second->clear(serverEngineID);
	//}

	boost::unique_lock< boost::shared_mutex > clearLock(callbackMutex);	//protects deviceIDsToClear list

	registeredDeviceCollector->getIDs(deviceIDsToClear);

	STI::TimingEngine::EngineInstance engineInstance(serverEngineID);

	EngineCallbackHandler_ptr serverClearCallback;
	EventEngineManager_ptr manager;
	for(STI::Device::DeviceIDSet::iterator id = deviceIDsToClear.begin(); id != deviceIDsToClear.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
			serverClearCallback = EngineCallbackHandler_ptr(
				new EngineCallbackHandler(*id, engineInstance, engineCallbackTarget));

			manager->clear(serverEngineID, serverClearCallback);
		}
	}

	//Clear ServerEventEngine structures
	deviceEventsIn.clear();
	deviceDependencies.clear();
	targetManagers->clear();
	partnerEvents->clear();
	
	//Sleep while waiting for devices to finish clearing
	waitForCallbacks(Clearing, deviceIDsToClear, parseTimeout_s);
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
			//This deviceID key is not in the map. Add a new TimingEventVector.
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
	STI::Device::DeviceIDSet_ptr deviceIDList;
	STI::Device::DeviceInterface_ptr device;
///	STI::Device::DeviceCollector_ptr partnerCollector;

	if(!deviceDependencies.contains(deviceID)) {
		//This is a new deviceID. Generate a new node and add it.
		newNode = ServerEventEngine::DeviceDependencyTreeNode_ptr(new ServerEventEngine::DeviceDependencyTreeNode(deviceID));
		deviceDependencies.add(deviceID, newNode);

		//Determine this node's device targets. Attempt to add them to the dependency list.
		if( registeredDeviceCollector->get(deviceID, device) && device != 0) {
			if(device->getEventPartners(deviceIDList) && deviceIDList != 0) {
//				partnerCollector->getIDs(deviceIDList);

				for(DeviceIDSet::iterator id = deviceIDList->begin(); id != deviceIDList->end(); id++) {
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
	bool found;
	
	for(DeviceIDSet::const_iterator deviceID = targetDevices.begin(); deviceID != targetDevices.end(); deviceID++) {
		found = registeredDeviceCollector->contains(*deviceID);
		allFound &= found;
		
		if(!found) {
			message << "Missing device: dev(" 
				<< deviceID->getName() << ", "
				<< deviceID->getAddress() << ", "
				<< deviceID->getModule() << ")" << std::endl;
		}
	}
	return allFound;
}

void ServerEventEngine::parse(const EngineTimestamp& parseTimeStamp, 
	const TimingEventVector_ptr& eventsIn, const ParsingResultsHandler_ptr& results)
{
	lastParseTimeStamp = parseTimeStamp;

	STI::TimingEngine::EngineInstance engineInstance(serverEngineID);
	engineInstance.parseTimestamp = lastParseTimeStamp;


	divideEventList(eventsIn, deviceEventsIn);	//eventsInByDevice

	determineDeviceDependencies(deviceDependencies);
	
	//Get all the devices that MAY have events.
	DeviceIDSet targetDevices;
	deviceDependencies.getKeys(targetDevices);	
	
	//Remove special targets that are treated separately at the end
	targetDevices.erase(serverID);
	targetDevices.erase(stiTriggerDeviceID);
	
	std::stringstream errMessage;

	//Check device availability
	if(!checkDeviceAvailability(targetDevices, errMessage)) {
		//Missing devices. Return with error message.
		results->returnResults(false, errMessage.str(), partnerEvents);
		return;
	}

	targetManagers->clear();

	//Parse events on all devices
	TimingEventVector_ptr deviceEvents;
	DeviceDependencyTreeNode_ptr deviceDependencyNode;
	unsigned currentParsingDeviceCount;
	std::set<DeviceID>::iterator targetID;

	using STI::TimingEngine::ParsingResultsHandler;
	STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler;

	boost::system_time wakeTime;
	
	parseSuccess = true;
	parsingErrors.str("");


	while(inState(Parsing) && targetDevices.size() > 0) 
	{
		//Lock parse results handler for an entire pass at the target devices.
		boost::unique_lock< boost::shared_mutex > parseLock(parseMutex);
		
		EventEngineManager_ptr manager;
//		TimingEventVector_ptr events;
		//Loop through all devices and attempt to parse if they have no depenendecies.
		for(targetID = targetDevices.begin(); targetID != targetDevices.end(); targetID++) {
			if(deviceDependencies.get(*targetID, deviceDependencyNode) && deviceDependencyNode != 0) {
				if(deviceDependencyNode->dependencyCount() == 0) {
					//Any device dependencies are gone, so this device is ready to parse.
					if(deviceEventsIn.get(*targetID, deviceEvents) && deviceEvents != 0 && deviceEvents->size() > 0) {

						//&& (*targetID) != serverID && (*targetID) != stiTriggerDeviceID

						//Verified that this device has events to parse. Now parse them.

						if(getDeviceEventEngineManager(*targetID, manager))
						{
							parsingHandler = ParsingResultsHandler_ptr(
								new ParsingResultsHandler(*targetID, engineInstance, serverParsingTarget));
					
							manager->parse(engineInstance, deviceEvents, parsingHandler);
							targetManagers->push_back(manager);
							
							parsingDevices.insert(*targetID);
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
		while( inState(Parsing) && parseSuccess && parsingDevices.size() > 0 
			&& currentParsingDeviceCount == parsingDevices.size())
		{
			wakeTime = boost::get_system_time() 
				+ boost::posix_time::milliseconds( static_cast<long>(parseTimeout_s * 1000) );
			parseCondition.timed_wait(parseMutex, wakeTime);
		}
	}
	
	//Parse local engine
	parsingHandler = ParsingResultsHandler_ptr(
		new ParsingResultsHandler(serverID, engineInstance, serverParsingTarget));
	if(!(deviceEventsIn.get(serverID, deviceEvents) && deviceEvents != 0))
		deviceEvents = TimingEventVector_ptr(new TimingEventVector());
	
	localEngine->parse(parseTimeStamp, deviceEvents, parsingHandler);
//	targetManagers->push_back(localEngineManager);


	//Look for remote trigger
	bool remoteTriggerFound = (
		deviceEventsIn.get(stiTriggerDeviceID, deviceEvents) && 
		deviceEvents != 0 && 
		setRemoteTriggerEngineManager(deviceEvents, triggerEngineManager));

	if(!remoteTriggerFound) {
		//Default; the local engine is the trigger
//		triggerEngineManager = localEngineManager;
	}


	results->returnResults(parseSuccess, parsingErrors.str(), partnerEvents);

	masterTrigger = MasterTrigger_ptr(new LocalMasterTrigger(targetManagers, engineInstance));


}

bool ServerEventEngine::setRemoteTriggerEngineManager(const TimingEventVector_ptr& specialEvents, 
													  EventEngineManager_ptr& triggerManager)
{
	if(specialEvents == 0)
		return false;

	bool foundTriggerManager = false;
	DeviceID triggerDeviceID("", "", 0);

	for(unsigned i = 0; i < specialEvents->size(); i++) {
		if(specialEvents->at(i)->channel().channelNum() == 1 && 
			specialEvents->at(i)->value().getType() == STI::Utils::String) {

			if(foundTriggerManager) {
				//Error: Multiple trigger registered
				parsingErrors << "Error: Multiple triggers registered." << std::endl 
				<< "       Only one trigger can be registered per timing file." << std::endl
				<< "       Location:" << std::endl
				<< "       >>> " << specialEvents->at(i)->position().file() << ", line " 
				<< specialEvents->at(i)->position().line() << "." << std::endl;
				return false;
			}

			//Trigger channel
			std::string triggerID = specialEvents->at(i)->value().getString();
			if(DeviceID::stringToDeviceID(triggerID, triggerDeviceID)) {
				foundTriggerManager = getDeviceEventEngineManager(triggerDeviceID, triggerManager);
			}
		}
	}

	return foundTriggerManager;
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

void ServerEventEngine::load(const EngineTimestamp& parseTimeStamp, const EngineCallbackHandler_ptr& callBack)
{
	using STI::TimingEngine::EngineCallbackHandler;
	using STI::TimingEngine::EngineCallbackHandler_ptr;
//	prepareForDevicesToLoad();

	STI::TimingEngine::EngineInstance engineInstance(serverEngineID);
	engineInstance.parseTimestamp = parseTimeStamp;

	localEngine->load(parseTimeStamp, callBack);

	boost::unique_lock< boost::shared_mutex > loadLock(callbackMutex);	//protects deviceIDsToLoad list

	deviceEventsIn.getKeys(deviceIDsToLoad);	//devices with events
	deviceIDsToLoad.erase(serverID);
	deviceIDsToLoad.erase(stiTriggerDeviceID);

	EngineCallbackHandler_ptr serverLoadCallBack;
	EventEngineManager_ptr manager;
	for(DeviceIDSet::iterator id = deviceIDsToLoad.begin(); id != deviceIDsToLoad.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
			serverLoadCallBack = EngineCallbackHandler_ptr(
				new EngineCallbackHandler(*id, engineInstance, engineCallbackTarget));

			manager->load(engineInstance, serverLoadCallBack);
		}
	}

//	waitForDevicesToLoad();
	//Sleep while waiting for devices to finish loading
	waitForCallbacks(Loading, deviceIDsToLoad, parseTimeout_s);
}

void ServerEventEngine::postLoad()
{
	localEngine->postLoad();
}

void ServerEventEngine::prePlay(const EngineTimestamp& parseTimeStamp, const EngineTimestamp& playTimeStamp, 
	const PlayOptions_ptr& playOptions, 
	const DocumentationOptions_ptr& docOptions, 
	const MeasurementResultsHandler_ptr& resultsHander, 
	const EngineCallbackHandler_ptr& callBack) 
{
	using STI::TimingEngine::LocalMasterTrigger;
	using STI::TimingEngine::MasterTrigger_ptr;
	using STI::TimingEngine::EngineCallbackHandler;
	using STI::TimingEngine::EngineCallbackHandler_ptr;

	currentPlayTimeStamp = playTimeStamp;

	localEngine->prePlay(parseTimeStamp, playTimeStamp, playOptions, docOptions, resultsHander, callBack);

	STI::TimingEngine::EngineInstance engineInstance(serverEngineID);
	engineInstance.parseTimestamp = parseTimeStamp;
	engineInstance.playTimestamp = playTimeStamp;

//	masterTrigger = MasterTrigger_ptr(new LocalMasterTrigger(targetManagers, engineInstance));

	boost::unique_lock< boost::shared_mutex > preparePlayLock(callbackMutex);	//protects deviceIDsToPlay list

	//Generate list of devices with events
	deviceEventsIn.getKeys(deviceIDsToPlay);
	deviceIDsToPlay.erase(serverID);
	deviceIDsToPlay.erase(stiTriggerDeviceID);

	EngineCallbackHandler_ptr serverPlayCallBack;
	EventEngineManager_ptr manager;
	for(STI::Device::DeviceIDSet::iterator id = deviceIDsToPlay.begin(); id != deviceIDsToPlay.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
			serverPlayCallBack = EngineCallbackHandler_ptr(
				new EngineCallbackHandler(*id, engineInstance, engineCallbackTarget));

			manager->play(engineInstance, playOptions, docOptions, resultsHander, serverPlayCallBack);
		}
	}

	////Sleep while waiting for devices to reach WaitingForTrigger
	//boost::system_time wakeTime;
	//while( localEngine->inState(PreparingToPlay) && deviceIDsToPlay.size() > 0 )
	//{
	//	wakeTime = boost::get_system_time() 
	//		+ boost::posix_time::milliseconds( static_cast<long>(parseTimeout_s * 1000) );
	//	callbackCondition.timed_wait(callbackMutex, wakeTime);
	//}
	
	//Sleep while waiting for devices to reach WaitingForTrigger
	waitForCallbacks(PreparingToPlay, deviceIDsToPlay, parseTimeout_s);
//localEngine->inState(PreparingToPlay) || localEngine->inState(WaitingForTrigger)
}

void ServerEventEngine::waitForCallbacks(const STI::TimingEngine::EventEngineState& localWaitState, 
										 const DeviceIDSet& devicesWithOurstandingCallbacks, double timeout_s)
{
	boost::system_time wakeTime;
	while( localEngine->inState(localWaitState) && devicesWithOurstandingCallbacks.size() > 0 )
	{
		wakeTime = boost::get_system_time() 
			+ boost::posix_time::milliseconds( static_cast<long>(timeout_s * 1000) );
		callbackCondition.timed_wait(callbackMutex, wakeTime);
	}
}

void ServerEventEngine::handleCallback(const STI::Device::DeviceID& deviceID, 
	const STI::TimingEngine::EngineInstance& engineInstance,
	const STI::TimingEngine::EventEngineState& state)
{
//	boost::unique_lock< boost::shared_mutex > parseLock(parseMutex);
	
	if(localEngine->inState(PreparingToPlay) ) {	//|| localEngine->inState(WaitingForTrigger)
		if(engineInstance.parseTimestamp != lastParseTimeStamp || 
			engineInstance.playTimestamp != currentPlayTimeStamp)
			return;

		//Waiting for all the engines to get into WaitingForTrigger state
		boost::unique_lock< boost::shared_mutex > preparePlayLock(callbackMutex);
		if(state == WaitingForTrigger || state == Triggered) {
			deviceIDsToPlay.erase(deviceID);
		}
		else {
			//Callback received, but the other engine is in the wrong state.
			stop();
			callbackCondition.notify_all();
		}
		if(deviceIDsToPlay.size() == 0) {
			callbackCondition.notify_one();
		}
	}
	else if(localEngine->inState(Loading)) {
		if(engineInstance.parseTimestamp != lastParseTimeStamp)
			return;

		boost::unique_lock< boost::shared_mutex > loadLock(callbackMutex);
		if(state == Loaded) {
			deviceIDsToLoad.erase(deviceID);
		}
		else {
			//Callback received, but the other engine is in the wrong state.
			stop();
			callbackCondition.notify_all();
		}
		if(deviceIDsToLoad.size() == 0) {
			callbackCondition.notify_one();
		}
	}
	else if(localEngine->inState(Playing)) {
		if(engineInstance.parseTimestamp != lastParseTimeStamp || 
			engineInstance.playTimestamp != currentPlayTimeStamp)
			return;
		boost::unique_lock< boost::shared_mutex > playLock(callbackMutex);
		//WaitingForTrigger will occur in repeat mode; indicates engine is ready to repeat
		//Loaded will occur in single run mode; indicates that the engine is finished playing
		if(state == WaitingForTrigger || state == Loaded) {
			deviceIDsToPlay.erase(deviceID);
		}
		else {
			//Callback received, but the other engine is in the wrong state.
			stop();
			callbackCondition.notify_all();
		}
		if(deviceIDsToPlay.size() == 0) {
			callbackCondition.notify_one();
		}
	}
	else if(localEngine->inState(Clearing)) {
		boost::unique_lock< boost::shared_mutex > clearLock(callbackMutex);
		if(state == Empty) {
			deviceIDsToClear.erase(deviceID);
		}
		else {
			//Callback received, but the other engine is in the wrong state.
			stop();
			callbackCondition.notify_all();
		}
		if(deviceIDsToClear.size() == 0) {
			callbackCondition.notify_one();
		}
	}
	//switch(localEngine->getState()) {
	//	case 
	//}
}

void ServerEventEngine::preTrigger(double startTime, double endTime)
{
	localEngine->preTrigger(startTime, endTime);
}

void ServerEventEngine::trigger(const MasterTrigger_ptr& delegatedTrigger)
{
	localEngine->trigger(delegatedTrigger);
	localEngine->trigger();
}

void ServerEventEngine::trigger()
{
	//bool triggeredByServer = true;

	//if(triggeredByServer) {
	//	trigger(masterTrigger);
	//}
	localEngine->trigger();
}

void ServerEventEngine::waitForTrigger(const EngineCallbackHandler_ptr& callBack)
{
	bool triggeredByServer = true;

	if(triggeredByServer) {
		trigger(masterTrigger);
	}
	//somehow need to call localEngine->waitForTrigger() when anothe device is the trigger.
	//Then the masterTrigger needs a reference to the server's EngineManager...
	localEngine->waitForTrigger(callBack);
}

void ServerEventEngine::play(const EngineTimestamp& parseTimeStamp, const EngineTimestamp& playTimeStamp, 
	const PlayOptions_ptr& playOptions, const DocumentationOptions_ptr& docOptions, 
	const MeasurementResultsHandler_ptr& resultsHander, const EngineCallbackHandler_ptr& callBack)
{
	boost::unique_lock< boost::shared_mutex > playLock(callbackMutex);	//protects deviceIDsToPlay list

	//Generate list of devices with events
	deviceEventsIn.getKeys(deviceIDsToPlay);
	deviceIDsToPlay.erase(serverID);
	deviceIDsToPlay.erase(stiTriggerDeviceID);

	localEngine->play(parseTimeStamp, playTimeStamp, playOptions, docOptions, resultsHander, callBack);

	//Sleep while waiting for devices to finish playing
	waitForCallbacks(Playing, deviceIDsToPlay, parseTimeout_s);
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

void ServerEventEngine::resume(const EngineCallbackHandler_ptr& callBack)
{
	localEngine->resume(callBack);
	
	STI::TimingEngine::EngineInstance engineInstance(serverEngineID);
	engineInstance.parseTimestamp = lastParseTimeStamp;

	STI::Device::DeviceIDSet deviceIDs;
	registeredDeviceCollector->getIDs(deviceIDs);
	EventEngineManager_ptr manager;
	for(STI::Device::DeviceIDSet::iterator id = deviceIDs.begin(); id != deviceIDs.end(); id++) {

		if(getDeviceEventEngineManager(*id, manager)) {
			manager->resume(engineInstance, callBack);
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

bool ServerEventEngine::publishData(const EngineTimestamp& timestamp, STI::TimingEngine::TimingMeasurementGroup_ptr& data, const 
									STI::TimingEngine::MeasurementResultsHandler_ptr& resultsHander, 
									const DocumentationOptions_ptr& documentation)
{
	return localEngine->publishData(timestamp, data, resultsHander, documentation);
}

